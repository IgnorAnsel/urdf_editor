#include "shape_relation.h"
#include "ui_shape_relation.h"
#include <QString>
std::vector<URDFJoint> joints;
shape_relation::shape_relation(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::shape_relation)
{
    ui->setupUi(this);
    //addShapesToTreeWidget(shapes, ui->treeWidget);
    setAcceptDrops(true);
    ui->treeWidget->installEventFilter(this);

}

shape_relation::~shape_relation()
{
    delete ui;
}
void shape_relation::addShapesToTreeWidget(const std::vector<Shape> &shapes, QTreeWidget *treeWidget)
{
    ui->treeWidget->clear();
    for (const auto &shape : shapes)
    {
        // 创建一个新的树形控件项，并设置第一列为形状的名称
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
        item->setText(0, QString::fromStdString(shape.link.name));
        // 将该项目添加到树形控件中
        treeWidget->addTopLevelItem(item);
    }
}
void shape_relation::update_shape()
{
    // 创建一个用于快速查找形状标识符的集合
    std::set<int> shapeIds;
    std::map<int, std::string> shapeNameMap; // 用于存储 id 到 name 的映射

    for (const auto &shape : shapes)
    {
        shapeIds.insert(shape.id);
        shapeNameMap[shape.id] = shape.link.name;
    }

    // 更新或添加树形控件中的节点
    for (const auto &shape : shapes)
    {
        bool found = false;
        // 使用递归函数查找并更新节点
        found = findAndUpdateNode(ui->treeWidget->invisibleRootItem(), shape.id, QString::fromStdString(shape.link.name));

        if (!found)
        {
            // 如果没有找到，添加为顶级节点
            QTreeWidgetItem *newItem = new QTreeWidgetItem(ui->treeWidget);
            newItem->setText(0, QString::fromStdString(shape.link.name));
            newItem->setData(0, Qt::UserRole, shape.id); // 存储唯一标识符
            ui->treeWidget->addTopLevelItem(newItem);
        }
    }

    // 删除不在 shapes 中的节点
    qDebug() << "yes";
    removeInvalidNodes(ui->treeWidget->invisibleRootItem(), shapeIds, shapeNameMap);

}

// 递归查找和更新节点
bool shape_relation::findAndUpdateNode(QTreeWidgetItem *parent, int id, const QString &name)
{
    for (int i = 0; i < parent->childCount(); ++i)
    {
        QTreeWidgetItem *child = parent->child(i);
        if (child->data(0, Qt::UserRole).toInt() == id)
        {
            // 如果找到了，更新节点的名称
            child->setText(0, name);
            return true;
        }
        // 递归查找子节点
        if (findAndUpdateNode(child, id, name))
        {
            return true;
        }
    }
    return false;
}

// 递归删除无效节点
void shape_relation::removeInvalidNodes(QTreeWidgetItem *parent, const std::set<int> &shapeIds, const std::map<int, std::string> &shapeNameMap)
{
    for (int i = parent->childCount() - 1; i >= 0; --i)
    {
        QTreeWidgetItem *child = parent->child(i);
        int id = child->data(0, Qt::UserRole).toInt();
        if (shapeIds.find(id) == shapeIds.end())
        {
            delete parent->takeChild(i);
        }
        else
        {
            // 更新节点名称以防止名称更改
            child->setText(0, QString::fromStdString(shapeNameMap.at(id)));
            // 递归检查子节点
            removeInvalidNodes(child, shapeIds, shapeNameMap);
        }
    }
}
void shape_relation::updateJointNames(QTreeWidgetItem* item, const QString& parentPath) {
    QString currentPath;

    // 如果 parentPath 为空，说明是顶级节点

    if (parentPath.isEmpty()) {
        currentPath = item->text(0);

    } else {
        // 仅使用父节点的名称构建 currentPath
        currentPath = parentPath.split("_to_").last() + "_to_" + item->text(0);
    }
    item->setText(1, currentPath);

    // 更新所有 shape 的 joint.name
    for (auto &shape : shapes) {
        shape.joint.name = currentPath.toStdString();
        qDebug() << shape.joint.name;
    }

    // 递归处理子节点
    for (int i = 0; i < item->childCount(); ++i) {
        updateJointNames(item->child(i), currentPath);
    }
}
void shape_relation::updateShapeIds(QTreeWidgetItem* root) {
    // 递归地更新每个节点的 ID 信息
    recursiveUpdateShapeIds(root, -1); // 假设顶级节点没有父节点，所以传递 -1
}

void shape_relation::recursiveUpdateShapeIds(QTreeWidgetItem* node, int parentId) {
    if (!node) return;

    int childId = node->data(0, Qt::UserRole).toInt(); // 假设每个节点的 child_id 存储在 UserRole

    // 更新对应的 shape
    for (auto &shape : shapes) {
        if (shape.id == childId) {
            shape.joint.child_id = childId;
            shape.joint.parent_id = parentId;
            qDebug() << "Updated shape ID " << shape.id
                     << " with parent_id: " << parentId
                     << " and child_id: " << childId;
            break;
        }
    }

    // 递归更新所有子节点
    for (int i = 0; i < node->childCount(); ++i) {
        recursiveUpdateShapeIds(node->child(i), childId); // 将当前节点的 ID 作为下一个调用的 parentId
    }
}

void shape_relation::copyItem()
{
    copiedShape = shapes[copiedId];
}

void shape_relation::pasteItem()
{
    Shape pasteShape = copiedShape;
    pasteShape.id = shapes.back().id+1;
    pasteShape.link.name = pasteShape.link.name.append(std::to_string(pasteShape.id));
    QTreeWidgetItem *newItem = new QTreeWidgetItem();
    newItem->setText(0, QString::fromStdString(pasteShape.link.name));
    newItem->setData(0, Qt::UserRole, pasteShape.id);

    QTreeWidgetItem *currentItem = ui->treeWidget->currentItem();
    if (currentItem)
    {
        if (currentItem->parent())
        {
            currentItem->parent()->addChild(newItem);
        }
        else
        {
            ui->treeWidget->addTopLevelItem(newItem);
        }
    }
    else
    {
        ui->treeWidget->addTopLevelItem(newItem);
    }
    shapes.push_back(pasteShape);
    qDebug() << "Pasted shape id:" << copiedShape.id;
    emit uptatepaste();
}


void shape_relation::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    // 获取点击项的文本信息
    QString text = item->text(column);
    qDebug() << "Clicked item text:" << text;

    // 获取点击项的唯一标识符（id）
    int id = item->data(0, Qt::UserRole).toInt();
    qDebug() << "Clicked item id:" << id;

    // 触发更新索引的信号，传递 id
    copiedId = id;
    emit updateInde(id);
}
void shape_relation::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug()<<2;

    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void shape_relation::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug()<<3;

    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void shape_relation::dropEvent(QDropEvent *event)
{
    // 检查拖放的数据是否包含文本
    if (event->mimeData()->hasText())
    {
        // 获取拖放的文本
        QString text = event->mimeData()->text();
        qDebug()<<text;
        event->acceptProposedAction();
    }
}
bool shape_relation::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->treeWidget) {
        // 检查是否是拖拽释放事件
        if (event->type() == QEvent::ChildRemoved) {
            // 更新所有顶级项目的父子关系
            for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
                QTreeWidgetItem* topLevelItem = ui->treeWidget->topLevelItem(i);
                updateShapeIds(topLevelItem);  // 确保这是正确的函数来更新 ID
                updateJointNames(topLevelItem, "");

            }
            // 可能还需要在这里调用 updateJointNames 如果需要更新名称
        }
    }

    // 默认情况下将事件传递给父类
    return QWidget::eventFilter(watched, event);
}


void shape_relation::keyPressEvent(QKeyEvent *event)
{

        if (event->matches(QKeySequence::Copy))
        {
        qDebug()<<"copy";
            copyItem();
        }
        else if (event->matches(QKeySequence::Paste))
        {
            pasteItem();
        }
        else
        {
            //QTreeWidget::keyPressEvent(event);
        }

}






void shape_relation::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    qDebug()<<num;
    num++;

}


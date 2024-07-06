#include "shape_relation.h"
#include "ui_shape_relation.h"
#include <QString>
std::vector<URDFJoint> joints;
std::map<int, std::string> shapeNameMap; // 用于存储 id 到 name 的映射
shape_relation::shape_relation(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::shape_relation)
{
    ui->setupUi(this);
    // addShapesToTreeWidget(shapes, ui->treeWidget);
    setAcceptDrops(true);
    ui->treeWidget->installEventFilter(this);
}

shape_relation::~shape_relation()
{
    delete ui;
}

void shape_relation::update_item()
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(i);
        updateJointNames(topLevelItem, "");
    }
}
void shape_relation::addShapesToTreeWidget(const std::vector<Shape> &shapes, QTreeWidget *treeWidget)
{
    ui->treeWidget->clear();
    for (const auto &shape : shapes)
    {
        // 创建一个新的树形控件项，并设置第一列为形状的名称
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
        item->setText(0, QString::fromStdString(shape.link.name));
        item->setData(0, Qt::UserRole, shape.id); // 存储形状 ID
        item->setData(1, Qt::UserRole, shape.joint.id); // 存储 joint ID
        // 将该项目添加到树形控件中
        treeWidget->addTopLevelItem(item);
    }
}

void shape_relation::reset()
{
    num = 0;
    joint_num = 0;
    ui->treeWidget->clear();
}

void shape_relation::update_shape()
{
    // 创建一个用于快速查找形状标识符的集合
    std::set<int> shapeIds;

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
            newItem->setData(1, Qt::UserRole, -1);
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
void shape_relation::updateJointNames(QTreeWidgetItem *item, const QString &parentPath)
{
    QString currentPath;

    // 如果 parentPath 为空，说明是顶级节点
    if (parentPath.isEmpty())
    {
        currentPath = item->text(0);
    }
    else
    {
        // 仅使用父节点的名称构建 currentPath
        currentPath = parentPath.split("_to_").last() + "_to_" + item->text(0);
    }

    // 找到对应的 shape，并更新其 joint.name
    int shapeId = item->data(0, Qt::UserRole).toInt();
    for (auto &shape : shapes)
    {
        if (shape.id == shapeId)
        {
            if(!shape.joint.joint_name_manset)
                {
                shape.joint.name = currentPath.toStdString();
                item->setText(1, currentPath);
                }
            else{
                item->setText(1,QString::fromStdString(shape.joint.name));
            }
            break;
        }
    }

    // 递归处理子节点
    for (int i = 0; i < item->childCount(); ++i)
    {
        updateJointNames(item->child(i), currentPath);
    }
}

void shape_relation::updateShapeIds(QTreeWidgetItem *root)
{
    // 递归地更新每个节点的 ID 信息
    recursiveUpdateShapeIds(root, -1); // 假设顶级节点没有父节点，所以传递 -1
}

void shape_relation::recursiveUpdateShapeIds(QTreeWidgetItem *node, int parentId)
{
    if (!node)
        return;

    int childId = node->data(0, Qt::UserRole).toInt(); // 假设每个节点的 child_id 存储在 UserRole

    // 更新对应的 shape
    for (auto &shape : shapes)
    {
        if (shape.id == childId)
        {
            // 确保 parentId 被正确传递和更新
            shape.joint.parent_id = parentId;
            shape.joint.child_id = childId;
            shape.joint.type = "fixed";
            shape.joint.parent_link = parentId != -1 ? shapeNameMap[parentId] : "";
            shape.joint.child_link = shapeNameMap[childId];
            shape.joint.id = joint_num++; // 设置 joint.id 并递增
            shape.isjointset = true;      // 设置标志位为 true
            shape.joint.axis.xyz = QVector3D(0.0f,0.0f,0.0f);
            shape.joint.limits.effort = 0;
            shape.joint.limits.lower = 0;
            shape.joint.limits.upper = 0;
            shape.joint.limits.velocity = 0;
            shape.joint.dynamics.damping = 0;
            shape.joint.dynamics.friction = 0;
            shape.joint.calibration.falling = 0;
            shape.joint.calibration.rising = 0;
            shape.joint.mimic.joint_name = "";
            shape.joint.mimic.multiplier = 0;
            shape.joint.mimic.offset = 0;
            shape.joint.safety_controller.soft_lower_limit = "";
            shape.joint.safety_controller.soft_upper_limit = "";
            shape.joint.safety_controller.k_position = 0;
            shape.joint.safety_controller.k_velocity = 0;
            qDebug() << "Updated shape ID " << shape.id
                     << " with parent_id: " << parentId
                     << " and child_id: " << childId
                     << " joint ID: " << shape.joint.id
                     << "joint Name:" << shape.joint.name;
            node->setData(1, Qt::UserRole, shape.joint.id); // 存储 joint id
            break;
        }
    }

    // 递归更新所有子节点
    for (int i = 0; i < node->childCount(); ++i)
    {
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
    pasteShape.id = shapes.back().id + 1;
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
    int id;
    if (column == 0)
    {
        id = item->data(0, Qt::UserRole).toInt();
        copiedId = id;
        emit updateInde(id);
    }
    else if (column == 1)
    {
        int joint_id = item->data(1, Qt::UserRole).toInt();
        qDebug() << joint_id;
        // 查找对应的 shape
        for (const auto &shape : shapes)
        {
            if (joint_id == -1)
            {
                id = item->data(0, Qt::UserRole).toInt();
                emit updateInde(id);
                break;
            }
            if (shape.joint.id == joint_id)
            {
                qDebug() << "Found shape with joint id:" << joint_id
                         << ", parent_id:" << shape.joint.parent_id
                         << ", child_id:" << shape.joint.child_id
                <<", joint_name:" << shape.joint.name;
                id = shape.joint.child_id;
                emit updateInde(id);
                emit updateJointIndex(joint_id);
                break;
            }
        }
    }
    // 触发更新索引的信号，传递 id
    qDebug() << "Clicked item id:" << id;
}
void shape_relation::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << 2;

    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void shape_relation::dragMoveEvent(QDragMoveEvent *event)
{
    qDebug() << 3;

    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
    else
    {
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
        qDebug() << text;
        event->acceptProposedAction();
    }
}
bool shape_relation::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->treeWidget)
    {
        // 检查是否是拖拽释放事件
        if (event->type() == QEvent::ChildRemoved)
        {
            // 更新所有顶级项目的父子关系
            for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
            {
                QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(i);
                updateShapeIds(topLevelItem); // 确保这是正确的函数来更新 ID
                updateJointNames(topLevelItem, "");
            }
            // 可能还需要在这里调用 updateJointNames 如果需要更新名称
        }
    }
    emit updateJoint();
    // 默认情况下将事件传递给父类
    return QWidget::eventFilter(watched, event);
}

void shape_relation::keyPressEvent(QKeyEvent *event)
{

    if (event->matches(QKeySequence::Copy))
    {
        qDebug() << "copy";
        copyItem();
    }
    else if (event->matches(QKeySequence::Paste))
    {
        pasteItem();
    }
    else
    {
        // QTreeWidget::keyPressEvent(event);
    }
}

void shape_relation::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    qDebug() << num;
    num++;
}

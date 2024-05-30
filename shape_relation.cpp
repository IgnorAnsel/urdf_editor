#include "shape_relation.h"
#include "ui_shape_relation.h"
#include <QString>
std::vector<URDFJoint> joints;
shape_relation::shape_relation(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::shape_relation)
{
    ui->setupUi(this);
    addShapesToTreeWidget(shapes, ui->treeWidget);
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
    ui->treeWidget->clear();
    for (const auto &shape : shapes)
    {
        // 创建一个新的树形控件项，并设置第一列为形状的名称
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, QString::fromStdString(shape.link.name));
        // 将该项目添加到树形控件中
        ui->treeWidget->addTopLevelItem(item);
    }
}

void shape_relation::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    // 获取点击项的文本信息
    QString text = item->text(column);
    qDebug() << "Clicked item text:" << text;
    // 获取行号
    int index = -1;
    index = ui->treeWidget->indexOfTopLevelItem(item);
    emit updateInde(index);
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
    qDebug()<<1;

    if (event->mimeData()->hasText())
    {
        // 获取拖放的文本
        QString text = event->mimeData()->text();
        qDebug()<<text;
//        // 在鼠标释放位置创建 URDFJoint
//        QPoint dropPos = event->pos();
//        URDFJoint *newJoint = new URDFJoint(this);

//        // 设置新 joint 的位置
//        newJoint->setGeometry(dropPos.x(), dropPos.y(), 100, 50);

//        // 设置新 joint 的属性
//        newJoint->setText("New Joint");

//        // 添加新 joint 到父项中
//        URDFLink *parentLink = dynamic_cast<URDFLink*>(itemAt(event->pos()));
//        if (parentLink) {
//            parentLink->addChild(newJoint);
//        }

//        // 显示新 joint
//        newJoint->show();

        // 接受拖放操作
        event->acceptProposedAction();
    }
}

bool shape_relation::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->treeWidget) {
            if(event->type()==QEvent::ChildRemoved){
            URDFJoint joint;
            for (const auto &shape : shapes)
            {

            }
            }
    }
    // 默认情况下将事件传递给父类
    return QWidget::eventFilter(watched, event);
}

void shape_relation::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    qDebug()<<num;
    num++;

}


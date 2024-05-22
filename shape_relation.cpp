#include "shape_relation.h"
#include "ui_shape_relation.h"
#include <QString>
shape_relation::shape_relation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::shape_relation)
{
    ui->setupUi(this);
    addShapesToTreeWidget(shapes,ui->treeWidget);
}

shape_relation::~shape_relation()
{
    delete ui;
}
void shape_relation::addShapesToTreeWidget(const std::vector<Shape>& shapes, QTreeWidget* treeWidget) {
    ui->treeWidget->clear();
    for(const auto& shape : shapes) {
        // 创建一个新的树形控件项，并设置第一列为形状的名称
        QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
        item->setText(0, QString::fromStdString(shape.link.name));
        // 将该项目添加到树形控件中
        treeWidget->addTopLevelItem(item);
    }
}
void shape_relation::update_shape(){
    ui->treeWidget->clear();
    for(const auto& shape : shapes) {
        // 创建一个新的树形控件项，并设置第一列为形状的名称
        QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, QString::fromStdString(shape.link.name));
        // 将该项目添加到树形控件中
        ui->treeWidget->addTopLevelItem(item);
    }
}

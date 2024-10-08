#include "geometry_choose.h"
#include "ui_geometry_choose.h"
geometry_choose::geometry_choose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::geometry_choose)
{
    ui->setupUi(this);

}

geometry_choose::~geometry_choose()
{
    delete ui;
}

void geometry_choose::on_listWidget_currentTextChanged(const QString &currentText)
{
    createShape(currentText);
}

void geometry_choose::numadd()
{
    num++;
}

void geometry_choose::shape_Created(const Shape &newShape)
{
    shapeCreated(newShape);
}
void geometry_choose::createShape(const QString &shapeType)
{
    Shape newShape;
    if (shapeType == "Box")
    {
        newShape = Shape(Shape::Cube);
        newShape.link.visuals.geometry.box.size = QVector3D(1.0f, 1.0f, 1.0f);
    }
    else if (shapeType == "Sphere")
    {
        newShape = Shape(Shape::Sphere);
        newShape.link.visuals.geometry.sphere.radius = 1.0f;
    }
    else if (shapeType == "Cylinder")
    {
        newShape = Shape(Shape::Cylinder);
        newShape.link.visuals.geometry.cylinder.radius = 1.0f;
        newShape.link.visuals.geometry.cylinder.length = 2.0f;
    }
    // 设置默认属性
    newShape.link.visuals.origin.xyz = QVector3D(0.0f, 0.0f, 0.0f);
    newShape.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    newShape.link.visuals.color = QColor(Qt::white);
    QString link_name = QString("base_").append(QString::number(num));
    newShape.link.name = link_name.toStdString();
    emit shapeCreated(newShape);
}


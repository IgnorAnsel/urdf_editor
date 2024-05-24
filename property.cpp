#include "property.h"
#include "ui_property.h"
#include "urdf_editor.h"
Property::Property(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Property)
{
    ui->setupUi(this);
    on_collision_geometry_type_currentTextChanged(ui->collision_geometry_type->currentText());
    on_visual_geometry_type_currentTextChanged(ui->visual_geometry_type->currentText());
}

void Property::createShape(const QString &shapeType)
{
    Shape newShape;
    newShape.link.iscreated = false;
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
    updateShapeProperties(newShape);
}

void Property::updateShape(int index)
{
    shapes[index].link.visuals.origin.xyz.setY(ui->visual_origin_y->text().toFloat());
    shapes[index].link.visuals.origin.xyz.setX(ui->visual_origin_x->text().toFloat());
    shapes[index].link.visuals.origin.xyz.setZ(ui->visual_origin_z->text().toFloat());
    shapes[index].link.visuals.origin.rpy.setX(ui->visual_origin_r->text().toFloat());
    shapes[index].link.visuals.origin.rpy.setY(ui->visual_origin_p->text().toFloat());
    shapes[index].link.visuals.origin.rpy.setZ(ui->visual_origin_y_2->text().toFloat());
    shapes[index].link.visuals.color.setRedF(ui->visual_color_r->text().toFloat());
    shapes[index].link.visuals.color.setGreenF(ui->visual_color_g->text().toFloat());
    shapes[index].link.visuals.color.setBlueF(ui->visual_color_b->text().toFloat());
    //currentShape.link.visuals.color.setAlphaF(ui->visual_color_a->text().toFloat());
    shapes[index].link.visuals.geometry.box.size.setX(ui->visual_box_l->text().toFloat());
    shapes[index].link.visuals.geometry.box.size.setY(ui->visual_box_w->text().toFloat());
    shapes[index].link.visuals.geometry.box.size.setZ(ui->visual_box_label->text().toFloat());
    shapes[index].link.visuals.geometry.sphere.radius = ui->visual_sphere_radius->text().toFloat();
    shapes[index].link.visuals.geometry.cylinder.radius = ui->visual_cylinder_radius->text().toFloat();
    shapes[index].link.visuals.geometry.cylinder.length = ui->visual_cylinder_length->text().toFloat();
    shapes[index].link.name = ui->link_name->text().toStdString();
    shapes[index].link.iscreated = false;
    updateShapeProperties(shapes[index]);
    emit updateshapes();
    //emit createshape();
}

Property::~Property()
{
    delete ui;
}

void Property::updateShapeProperties(const Shape &shape)
{
    currentShape = shape;
    // 更新UI以显示形状的属性
    // if(shape.link.iscreated)
    //     ui->link_name->setText(QString::fromStdString(shape.link.name));
    // else
    //     ui->link_name->setText("");
    ui->visual_origin_x->setText(QString::number(shape.link.visuals.origin.xyz.x()));
    ui->visual_origin_y->setText(QString::number(shape.link.visuals.origin.xyz.y()));
    ui->visual_origin_z->setText(QString::number(shape.link.visuals.origin.xyz.z()));

    ui->visual_origin_r->setText(QString::number(shape.link.visuals.origin.rpy.x()));
    ui->visual_origin_p->setText(QString::number(shape.link.visuals.origin.rpy.y()));
    ui->visual_origin_y_2->setText(QString::number(shape.link.visuals.origin.rpy.z()));

    ui->visual_color_r->setText(QString::number(shape.link.visuals.color.redF()));
    ui->visual_color_g->setText(QString::number(shape.link.visuals.color.greenF()));
    ui->visual_color_b->setText(QString::number(shape.link.visuals.color.blueF()));
    //ui->visual_color_a->setText(QString::number(shape.link.visuals.color.alphaF()));

    ui->visual_box_l->setText(QString::number(shape.link.visuals.geometry.box.size.x()));
    ui->visual_box_w->setText(QString::number(shape.link.visuals.geometry.box.size.y()));
    ui->visual_box_h->setText(QString::number(shape.link.visuals.geometry.box.size.z()));

    ui->visual_sphere_radius->setText(QString::number(shape.link.visuals.geometry.sphere.radius));

    ui->visual_cylinder_radius->setText(QString::number(shape.link.visuals.geometry.cylinder.radius));
    ui->visual_cylinder_length->setText(QString::number(shape.link.visuals.geometry.cylinder.length));
    if (shape.type == Shape::Cube)
    {
        ui->visual_geometry_type->setCurrentText("Box");
        ui->collision_geometry_type->setCurrentText("Box");
    }
    else if (shape.type == Shape::Sphere)
    {
        ui->visual_geometry_type->setCurrentText("Sphere");
        ui->collision_geometry_type->setCurrentText("Sphere");

    }
    else if (shape.type == Shape::Cylinder)
    {
        ui->visual_geometry_type->setCurrentText("Cylinder");
        ui->collision_geometry_type->setCurrentText("Cylinder");
    }
}

void Property::receiveindex(int index)
{
    if(index>=0)
    {
        shapes[index].link.iscreated =true;
        currentSetlectShape = shapes[index];
        currentIndex = index;
        updateShapeProperties(currentSetlectShape);
        ui->pushButton->hide();
    }
}

void Property::on_collision_geometry_type_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Box") {
        ui->collision_geometry_cylinder->hide();
        ui->collision_geometry_sphere->hide();
        ui->collision_geometry_box->show();
    } else if (arg1 == "Cylinder") {
        ui->collision_geometry_box->hide();
        ui->collision_geometry_sphere->hide();
        ui->collision_geometry_cylinder->show();
    } else if (arg1 == "Sphere") {
        ui->collision_geometry_box->hide();
        ui->collision_geometry_cylinder->hide();
        ui->collision_geometry_sphere->show();
    }
}

void Property::on_visual_geometry_type_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Box") {
        ui->visual_geometry_cylinder->hide();
        ui->visual_geometry_sphere->hide();
        ui->visual_geometry_box->show();
    } else if (arg1 == "Cylinder") {
        ui->visual_geometry_box->hide();
        ui->visual_geometry_sphere->hide();
        ui->visual_geometry_cylinder->show();
    } else if (arg1 == "Sphere") {
        ui->visual_geometry_box->hide();
        ui->visual_geometry_cylinder->hide();
        ui->visual_geometry_sphere->show();
    }
}



void Property::on_link_name_textChanged(const QString &arg1)
{
    currentShape.link.name = arg1.toStdString();
}


void Property::on_visual_origin_y_textChanged(const QString &arg1)
{
    currentShape.link.visuals.origin.xyz.setY(ui->visual_origin_y->text().toFloat());
    if(currentIndex>=0)
    updateShape(currentIndex);
}

void Property::on_visual_origin_x_editingFinished()
{
    currentShape.link.visuals.origin.xyz.setX(ui->visual_origin_x->text().toFloat());
    if(currentIndex>=0)
    updateShape(currentIndex);

}


void Property::on_visual_origin_z_editingFinished()
{
    currentShape.link.visuals.origin.xyz.setZ(ui->visual_origin_z->text().toFloat());
    if(currentIndex>=0)
    updateShape(currentIndex);
}


void Property::on_pushButton_clicked()
{
    currentShape.link.visuals.origin.xyz.setY(ui->visual_origin_y->text().toFloat());
    currentShape.link.visuals.origin.xyz.setX(ui->visual_origin_x->text().toFloat());
    currentShape.link.visuals.origin.xyz.setZ(ui->visual_origin_z->text().toFloat());
    currentShape.link.visuals.origin.rpy.setX(ui->visual_origin_r->text().toFloat());
    currentShape.link.visuals.origin.rpy.setY(ui->visual_origin_p->text().toFloat());
    currentShape.link.visuals.origin.rpy.setZ(ui->visual_origin_y_2->text().toFloat());
    currentShape.link.visuals.color.setRedF(ui->visual_color_r->text().toFloat());
    currentShape.link.visuals.color.setGreenF(ui->visual_color_g->text().toFloat());
    currentShape.link.visuals.color.setBlueF(ui->visual_color_b->text().toFloat());
    //currentShape.link.visuals.color.setAlphaF(ui->visual_color_a->text().toFloat());
    currentShape.link.visuals.geometry.box.size.setX(ui->visual_box_l->text().toFloat());
    currentShape.link.visuals.geometry.box.size.setY(ui->visual_box_w->text().toFloat());
    currentShape.link.visuals.geometry.box.size.setZ(ui->visual_box_label->text().toFloat());
    currentShape.link.visuals.geometry.sphere.radius = ui->visual_sphere_radius->text().toFloat();
    currentShape.link.visuals.geometry.cylinder.radius = ui->visual_cylinder_radius->text().toFloat();
    currentShape.link.visuals.geometry.cylinder.length = ui->visual_cylinder_length->text().toFloat();
    currentShape.link.name = ui->link_name->text().toStdString();
    // 添加到形状列表
    if(currentShape.link.visuals.geometry.box.size==QVector3D(0.0f,0.0f,0.0f)&&currentShape.type==Shape::Cube)
        return;
    if(currentShape.link.name.empty())
    {
        currentShape.link.name = QString("base_").append(QString::number(num)).toStdString();
        num++;
    }
    for(const auto& shape : shapes) {
        if(shape.link.name == currentShape.link.name) {
            QMessageBox::information(NULL, "ERROR", "The name of the link is already used. Please choose a different name.", QMessageBox::Ok);
            return;
        }
    }
    updateShapeProperties(currentShape);
    shapes.push_back(currentShape);
    emit createshape();
    //emit updateshape(currentShape);
//    emit changenum();
}


void Property::on_listWidget_currentTextChanged(const QString &currentText)
{
    ui->pushButton->show();
    currentIndex = -1;
    createShape(currentText);
}


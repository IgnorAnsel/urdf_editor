#include "property.h"
#include "ui_property.h"

Property::Property(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Property)
{
    ui->setupUi(this);
    on_collision_geometry_type_currentTextChanged(ui->collision_geometry_type->currentText());
    on_visual_geometry_type_currentTextChanged(ui->visual_geometry_type->currentText());
}

Property::~Property()
{
    delete ui;
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



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

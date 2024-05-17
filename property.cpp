#include "property.h"
#include "ui_property.h"

property::property(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::property)
{
    ui->setupUi(this);
}

property::~property()
{
    delete ui;
}

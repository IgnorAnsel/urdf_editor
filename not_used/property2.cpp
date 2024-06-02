#include "property2.h"
#include "ui_property2.h"

property2::property2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::property2)
{
    ui->setupUi(this);
}

property2::~property2()
{
    delete ui;
}

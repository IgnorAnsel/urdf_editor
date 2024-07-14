#include "base_setting.h"
#include "ui_base_setting.h"

base_setting::base_setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::base_setting)
{
    ui->setupUi(this);
}

base_setting::~base_setting()
{
    delete ui;
}

void base_setting::on_comboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Light")
        emit choose_drak_light(0);
    else if(arg1 == "Dark")
        emit choose_drak_light(1);
    else if(arg1 == "Blue")
        emit choose_drak_light(2);
    //emit updatepic();
}


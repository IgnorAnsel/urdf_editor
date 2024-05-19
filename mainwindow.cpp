#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include "collapsiblewidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(300,300,1600,1000);
    ui->widget_2->resize(QSize(440,1000));
    // 绑定动作
    connect(ui->actionNew, &QAction::triggered, actionHandler, &ActionHandler::newFile);
    connect(ui->actionOpen, &QAction::triggered, actionHandler, &ActionHandler::openFile);
    connect(ui->actionSave, &QAction::triggered, actionHandler, &ActionHandler::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, actionHandler, &ActionHandler::saveasFile);
    connect(ui->actionExit, &QAction::triggered, actionHandler, &ActionHandler::Exit);
    //创建垂直布局
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    QVBoxLayout *Layout = new QVBoxLayout();
    QVBoxLayout *Layout_choose = new QVBoxLayout();
    QVBoxLayout *Layout2 = new QVBoxLayout();
    pro = new Property();
    geometry = new geometry_choose;
    pro2 = new property2();

    //Layout->addWidget(pro);
    //ui->widget_param->setLayout(Layout);

    Layout_choose->addWidget(geometry);
    ui->widget_geometry->setLayout(Layout_choose);

    Layout2->addWidget(pro2);
    ui->widget_param->setLayout(Layout2);

    urdf_editor = new Urdf_editor(ui->widget);
    ui->widget->setLayout(verticalLayout);
    verticalLayout->addWidget(urdf_editor);

    // 最后，设置 ui->widget 的布局
    ui->widget->setLayout(verticalLayout);

}

MainWindow::~MainWindow()
{
    delete ui;
}


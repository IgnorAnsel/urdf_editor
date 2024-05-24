#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
//#include "collapsiblewidget.h"

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
    pro2 = new property2();
    tree = new shape_relation();
    Layout->addWidget(pro);
    ui->widget_param->setLayout(Layout);
    Layout2->addWidget(tree);
    ui->widget_3->setLayout(Layout2);
//    Layout2->addWidget(pro2);
//    ui->widget_param->setLayout(Layout2);
    urdf_editor = new Urdf_editor(ui->widget);
    ui->widget->setLayout(verticalLayout);
    verticalLayout->addWidget(urdf_editor);
    // 最后，设置 ui->widget 的布局
    ui->widget->setLayout(verticalLayout);
    //
    connect(pro,&Property::createshape,urdf_editor,&Urdf_editor::updateShape);
    connect(pro,&Property::createshape,tree,&shape_relation::update_shape);
    connect(pro,&Property::updateshape,urdf_editor,&Urdf_editor::updateShape);
    connect(pro,&Property::updateshapes,urdf_editor,&Urdf_editor::updateShape);
    connect(tree,&shape_relation::updateInde,urdf_editor,&Urdf_editor::receiveIndex);
    connect(tree,&shape_relation::updateInde,pro,&Property::receiveindex);
}

MainWindow::~MainWindow()
{
    delete ui;
}


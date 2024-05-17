#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include "collapsiblewidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setCentralWidget(ui->widget);
    //创建一个垂直布局
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    CollapsibleWidget *collapsibleWidget = new CollapsibleWidget("My Collapsible Section", this);
    CollapsibleWidget *collapsibleWidget1 = new CollapsibleWidget("My Collapsible Section", this);
    CollapsibleWidget *collapsibleWidget2 = new CollapsibleWidget("My Collapsible Section", this);

    // 创建一个垂直布局并将其设置到ui->widget
    QVBoxLayout *layout = new QVBoxLayout(ui->widget_2);
    // 将CollapsibleWidget插入到布局的顶部
    layout->insertWidget(0, collapsibleWidget);
    layout->insertWidget(1, collapsibleWidget1);
    layout->insertWidget(2, collapsibleWidget2);

    // 确保ui-&gt;widget使用这个布局
    ui->widget_2->setLayout(layout);
    // 绑定动作
    connect(ui->actionNew, &QAction::triggered, actionHandler, &ActionHandler::newFile);
    connect(ui->actionOpen, &QAction::triggered, actionHandler, &ActionHandler::openFile);
    connect(ui->actionSave, &QAction::triggered, actionHandler, &ActionHandler::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, actionHandler, &ActionHandler::saveasFile);
    connect(ui->actionExit, &QAction::triggered, actionHandler, &ActionHandler::Exit);
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


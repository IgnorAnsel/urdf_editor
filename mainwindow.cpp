#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionMoveRotateStep->setText("MStep\n"+QString::number(MStep));
    ui->actionWHLRStep->setText("WHLRStep\n");
    ui->actionshape->setText("Choose Shape");
    setGeometry(300, 300, 1600, 1000);
    this->setStatusBar(status);
    status->showMessage("请新建或打开文件");
    status->addPermanentWidget(statusLabel);//添加右侧标签(永久性)
    ui->widget_2->resize(QSize(440, 1000));
    connect(actionHandler,&ActionHandler::changestatus,this,&MainWindow::changeMainstatus);
    connect(actionHandler,&ActionHandler::clear,this,&MainWindow::clear);
    // 绑定动作
    connect(ui->actionNew, &QAction::triggered, actionHandler, &ActionHandler::newFile);
    connect(ui->actionOpen, &QAction::triggered, actionHandler, &ActionHandler::openFile);
    connect(ui->actionSave, &QAction::triggered, actionHandler, &ActionHandler::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, actionHandler, &ActionHandler::saveasFile);
    connect(ui->actionExit, &QAction::triggered, actionHandler, &ActionHandler::Exit);
    connect(ui->actionBase_Setting,&QAction::triggered,actionHandler,&ActionHandler::b_setting);

    // 创建垂直布局
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    QVBoxLayout *Layout = new QVBoxLayout();
    QVBoxLayout *Layout_choose = new QVBoxLayout();
    QVBoxLayout *Layout2 = new QVBoxLayout();
    pro = new Property();
    //    pro2 = new property2();
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
    connect(pro, &Property::createshape, urdf_editor, &Urdf_editor::updateShape);
    connect(pro, &Property::createshape, tree, &shape_relation::update_shape);
    connect(pro, &Property::updateshape, urdf_editor, &Urdf_editor::updateShape);
    connect(pro, &Property::updateshapes, urdf_editor, &Urdf_editor::updateShape);
    connect(pro, &Property::updateshapes, tree, &shape_relation::update_shape);
    connect(pro, &Property::drapcreate, urdf_editor, &Urdf_editor::dropCreate);
    connect(tree, &shape_relation::updateInde, urdf_editor, &Urdf_editor::receiveIndex);
    connect(tree, &shape_relation::updateInde, pro, &Property::receiveindex);
    connect(urdf_editor,&Urdf_editor::updateIndex,pro, &Property::receiveindex);
    connect(tree,&shape_relation::updateInde,this, &MainWindow::revicev_index);
    connect(tree, &shape_relation::uptatepaste, urdf_editor, &Urdf_editor::updateShape);
    connect(tree, &shape_relation::updateJointIndex, pro, &Property::receivejointindex);
    connect(tree,&shape_relation::updateJoint,urdf_editor,&Urdf_editor::updateJoint);
    connect(pro,&Property::update_items,tree,&shape_relation::update_item);
    connect(actionHandler,&ActionHandler::set_selectedShapeIndex,urdf_editor,&Urdf_editor::set_set_selectedShapeIndex_f1);
    connect(actionHandler,&ActionHandler::update,pro,&Property::formfileupdate);
    connect(actionHandler,&ActionHandler::update,tree,&shape_relation::updateItemSecondColumn);
    connect(this,&MainWindow::MoveRotate,urdf_editor,&Urdf_editor::ChangeMoveRotate);
    connect(actionHandler->bs,&base_setting::choose_drak_light,this,&MainWindow::choose_drak_light);
    connect(this,&MainWindow::updateWHLRStep,urdf_editor,&Urdf_editor::updateWHLRStep);
}

MainWindow::~MainWindow()
{
    QApplication::closeAllWindows();
    delete ui;
}

void MainWindow::changeMainstatus(QString path)
{
    status->showMessage(path);
    statusLabel->clear();
    statusLabel->setText(path);
    status->addPermanentWidget(statusLabel);//添加右侧标签(永久性)
}

void MainWindow::clear()
{
    pro->reset();
    tree->reset();
    urdf_editor->reset();
    update();
}

void MainWindow::update_zhuti()
{
    QString path = ":/pic/";
    if(dark_light_mode==1)
        path = path + "light";
    else if(dark_light_mode==0)
        path = path + "dark";
    else if(dark_light_mode==2)
        path = path + "dark";
    if(lastpic==0)
        ui->actionshape->setIcon(QIcon(path+"/Cube.png"));
    else if(lastpic==1)
        ui->actionshape->setIcon(QIcon(path+"/Cyliner.png"));
    else if(lastpic==2)
        ui->actionshape->setIcon(QIcon(path+"/Sphere.png"));
    else;
}

void MainWindow::revicev_index(int index)
{
    QString path = ":/pic/";
    if(dark_light_mode==1)
        path = path + "light";
    else if(dark_light_mode==0)
        path = path + "dark";
    else if(dark_light_mode==2)
        path = path + "dark";
    if(index>=0)
    {
        if(shapes[index].type == Shape::Cube)
        {
            ui->actionshape->setIcon(QIcon(path+"/Cube.png"));
            WHLRMode = 0;
            lastpic = 0;
        }
        if(shapes[index].type == Shape::Cylinder)
        {
            ui->actionshape->setIcon(QIcon(path+"/Cyliner.png"));
            WHLRMode = 1;
            lastpic = 1;
        }
        if(shapes[index].type == Shape::Sphere)
        {
            ui->actionshape->setIcon(QIcon(path+"/Sphere.png"));
            WHLRMode = 2;
            lastpic = 2;
        }
    }
}
void MainWindow::choose_drak_light(int dl)
{
    dark_light_mode = dl;
    QString styleSheet;
    if (dark_light_mode==0) {
        styleSheet = ":/qss/light.qss";
    }
    else if(dark_light_mode==1){
        styleSheet = ":/qss/dark.qss";
    }
    else if(dark_light_mode==2)
    {
        styleSheet = ":/qss/blue.qss";
    }
    QFile file(styleSheet);
    if (file.open(QFile::ReadOnly)) {
        QString style = QLatin1String(file.readAll());
        qApp->setStyleSheet(style);
        file.close();
    }
    update_zhuti();
}

void MainWindow::setToolBar()
{

}

void MainWindow::on_actionMoveRotate_toggled(bool arg1)
{
    //0 Move
    //1 Rotate
    setmode = arg1;
    emit MoveRotate(arg1);
    if(arg1)
        ui->actionMoveRotateStep->setText("RStep\n"+QString::number(RStep));
    else
        ui->actionMoveRotateStep->setText("MStep\n"+QString::number(MStep));

}


void MainWindow::on_actionMoveRotateStep_triggered()
{
    QDialog *set = new QDialog();

    QGridLayout *layout = new QGridLayout(set);
    QSlider *slider = new QSlider(set);
    QLabel *text = new QLabel(set);
    QLineEdit *edit = new QLineEdit(set);
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    set->setMaximumSize(400,400);
    set->setGeometry(500, 500, 200,50);
    slider->setOrientation(Qt::Horizontal);
    layout->addWidget(slider,0,0,1,2);
    layout->addWidget(text,1,0,1,1);
    layout->addWidget(edit,1,1,1,1);
    layout->addWidget(okButton,2,0,1,1);
    layout->addWidget(cancelButton,2,1,1,1);

    connect(slider, &QSlider::valueChanged, [=](int value){
        text->setText(QString::number(value / 200.0));
        edit->setText(QString::number(value / 200.0));
        //MainWindow::changeStep(value);
    });
    //connect(slider, &QSlider::valueChanged,this,&MainWindow::changeStep);
    connect(edit,&QLineEdit::textChanged,[=](QString s){
        text->setText(QString::number(s.toFloat()));
        slider->setValue(s.toFloat()*200);
        //MainWindow::changeStep(s.toFloat()*200);
    });
    connect(okButton,&QPushButton::pressed,[=](){
        int step = text->text().toFloat() * 200;
        urdf_editor->changeStep(step);
        MainWindow::changeStep(step);
        set->accept();
    });
    connect(cancelButton,&QPushButton::pressed,set,&QDialog::reject);
    if(setmode)
    {
        slider->setMaximum(800);
        slider->setValue(RStep*200);
        set->setWindowTitle("RotateSTEP");
    }
    else
    {
        slider->setMaximum(1000);
        slider->setValue(MStep*200);
        set->setWindowTitle("MoveSTEP");
    }
    text->setText(QString::number(slider->value()/200.0));
    edit->setText(QString::number(slider->value()/200.0));
    set->exec();
}

void MainWindow::changeStep(int value)
{
    if(setmode)
    {
        RStep = value/200.0;
        ui->actionMoveRotateStep->setText("RStep\n"+QString::number(RStep));
    }
    else
    {
        MStep = value/200.0;
        ui->actionMoveRotateStep->setText("MStep\n"+QString::number(MStep));
    }
}


void MainWindow::on_actionWHLRStep_triggered()
{
    QDialog *set = new QDialog();
    QGridLayout *layout = new QGridLayout(set);

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    set->setMaximumSize(400,400);
    set->setGeometry(500, 500, 200, 50);
    layout->addWidget(okButton, 4, 0, 1, 1);
    layout->addWidget(cancelButton, 4, 1, 1, 1);

    if (WHLRMode == 0) // Cube
    {
        QSlider *lengthSlider = new QSlider(set);
        QSlider *widthSlider = new QSlider(set);
        QSlider *heightSlider = new QSlider(set);
        QLineEdit *lengthEdit = new QLineEdit(set);
        QLineEdit *widthEdit = new QLineEdit(set);
        QLineEdit *heightEdit = new QLineEdit(set);
        lengthSlider->setValue(Cube_L*200);
        widthSlider->setValue(Cube_W*200);
        heightSlider->setValue(Cube_H*200);
        lengthEdit->setText(QString::number(Cube_L));
        widthEdit->setText(QString::number(Cube_W));
        heightEdit->setText(QString::number(Cube_H));
        lengthSlider->setOrientation(Qt::Horizontal);
        widthSlider->setOrientation(Qt::Horizontal);
        heightSlider->setOrientation(Qt::Horizontal);

        layout->addWidget(new QLabel("Length"), 0, 0, 1, 1);
        layout->addWidget(lengthSlider, 0, 1, 1, 2);
        layout->addWidget(lengthEdit, 0, 4, 1, 1);

        layout->addWidget(new QLabel("Width"), 1, 0, 1, 1);
        layout->addWidget(widthSlider, 1, 1, 1, 2);
        layout->addWidget(widthEdit, 1, 4, 1, 1);

        layout->addWidget(new QLabel("Height"), 2, 0, 1, 1);
        layout->addWidget(heightSlider, 2, 1, 1, 2);
        layout->addWidget(heightEdit, 2, 4, 1, 1);

        connect(lengthSlider, &QSlider::valueChanged, [=](int value){
            QString val = QString::number(value / 200.0);
            lengthEdit->setText(val);
        });
        connect(widthSlider, &QSlider::valueChanged, [=](int value){
            QString val = QString::number(value / 200.0);
            widthEdit->setText(val);
        });
        connect(heightSlider, &QSlider::valueChanged, [=](int value){
            QString val = QString::number(value / 200.0);
            heightEdit->setText(val);
        });

        connect(lengthEdit, &QLineEdit::textChanged, [=](const QString &s){
            float val = s.toFloat();
            lengthSlider->setValue(val * 200);
        });
        connect(widthEdit, &QLineEdit::textChanged, [=](const QString &s){
            float val = s.toFloat();
            widthSlider->setValue(val * 200);
        });
        connect(heightEdit, &QLineEdit::textChanged, [=](const QString &s){
            float val = s.toFloat();
            heightSlider->setValue(val * 200);
        });

        connect(okButton, &QPushButton::pressed, [=](){
            // 使用长度、宽度、高度进行必要的操作
            this->Cube_H = heightEdit->text().toDouble();
            this->Cube_W = widthEdit->text().toDouble();
            this->Cube_L = lengthEdit->text().toDouble();
            set->accept();
        });

        set->setWindowTitle("Cube Dimensions");
    }
    else if (WHLRMode == 1) // Cylinder
    {
        QSlider *radiusSlider = new QSlider(set);
        QSlider *heightSlider = new QSlider(set);
        QLineEdit *radiusEdit = new QLineEdit(set);
        QLineEdit *heightEdit = new QLineEdit(set);
        radiusSlider->setValue(Cyliner_R*200);
        heightSlider->setValue(Cyliner_H*200);
        radiusEdit->setText(QString::number(Cyliner_R));
        heightEdit->setText(QString::number(Cyliner_H));
        radiusSlider->setOrientation(Qt::Horizontal);
        heightSlider->setOrientation(Qt::Horizontal);

        layout->addWidget(new QLabel("Radius"), 0, 0, 1, 1);
        layout->addWidget(radiusSlider, 0, 1, 1, 2);
        layout->addWidget(radiusEdit, 0, 4, 1, 1);

        layout->addWidget(new QLabel("Height"), 1, 0, 1, 1);
        layout->addWidget(heightSlider, 1, 1, 1, 2);
        layout->addWidget(heightEdit, 1, 4, 1, 1);

        connect(radiusSlider, &QSlider::valueChanged, [=](int value){
            QString val = QString::number(value / 200.0);
            radiusEdit->setText(val);
        });
        connect(heightSlider, &QSlider::valueChanged, [=](int value){
            QString val = QString::number(value / 200.0);
            heightEdit->setText(val);
        });

        connect(radiusEdit, &QLineEdit::textChanged, [=](const QString &s){
            float val = s.toFloat();
            radiusSlider->setValue(val * 200);
        });
        connect(heightEdit, &QLineEdit::textChanged, [=](const QString &s){
            float val = s.toFloat();
            heightSlider->setValue(val * 200);
        });

        connect(okButton, &QPushButton::pressed, [=](){
            this->Cyliner_H = heightEdit->text().toDouble();
            this->Cyliner_R = radiusEdit->text().toDouble();
            // 使用半径和高度进行必要的操作
            set->accept();
        });

        set->setWindowTitle("Cylinder Dimensions");
    }
    else if (WHLRMode == 2) // Sphere
    {
        QSlider *radiusSlider = new QSlider(set);
        QLineEdit *radiusEdit = new QLineEdit(set);

        radiusSlider->setOrientation(Qt::Horizontal);
        layout->addWidget(new QLabel("Radius"), 0, 0, 1, 1);
        layout->addWidget(radiusSlider, 0, 1, 1, 2);
        layout->addWidget(radiusEdit, 0, 4, 1, 1);
        radiusSlider->setValue(Sphere_R*200);
        connect(radiusSlider, &QSlider::valueChanged, [=](int value){
            QString val = QString::number(value / 200.0);
            radiusEdit->setText(val);
        });

        connect(radiusEdit, &QLineEdit::textChanged, [=](const QString &s){
            float val = s.toFloat();
            radiusSlider->setValue(val * 200);
        });

        connect(okButton, &QPushButton::pressed, [=](){
            // 使用半径进行必要的操作
            this->Sphere_R = radiusEdit->text().toDouble();
            set->accept();
        });

        set->setWindowTitle("Sphere Dimensions");
    }

    connect(cancelButton, &QPushButton::pressed, set, &QDialog::reject);
    set->exec();
}



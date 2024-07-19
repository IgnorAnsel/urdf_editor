#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "actionhandler.h"
#include "urdf_editor.h"
#include "property.h"
#include <QLabel>
#include <QStatusBar>
#include <QQueue>
#include <QTimer>
// #include "property2.h"
#include "shape_relation.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QTimer *timer = new QTimer(this);
    ~MainWindow();
public slots:
    void changeMainstatus(QString path);
    void revicev_index(int index);
    void choose_drak_light(int dl);
    void clear();
private slots:
    void R_KeyPress(int keyvalue);
    void update_zhuti();
    void on_actionMoveRotate_toggled(bool arg1);
    void on_actionMoveRotateStep_triggered();
    void changeStep(int value);
    void on_actionWHLRStep_triggered();
    void PopKey();
private:
    QQueue<QString> queue;
    float RStep = 0.1;
    float MStep = 0.5;
    float Cube_W = 0.1;
    float Cube_H = 0.1;
    float Cube_L = 0.1;
    float Cyliner_H = 0.1;
    float Cyliner_R = 0.1;
    float Sphere_R = 0.1;
    int WHLRMode = 0; //
    bool setmode = 0;
    int dark_light_mode = 1 ;
    int lastpic;
    QLabel *statusLabel = new QLabel("请新建或打开文件", this);
    QStatusBar *status = new QStatusBar(this);
    Ui::MainWindow *ui;
    ActionHandler *actionHandler = new ActionHandler(this);
    Urdf_editor *urdf_editor;
    int timeout = 1000;
    Property *pro;
    // property2 *pro2;
    shape_relation *tree;
    void setToolBar();
signals:
    void MoveRotate(bool mode);
    void updateWHLRStep(float Cube_W ,
                        float Cube_H ,
                        float Cube_L ,
                        float Cyliner_H ,
                        float Cyliner_R ,
                        float Sphere_R );
    void ShapeKind(int kind);
};
#endif // MAINWINDOW_H

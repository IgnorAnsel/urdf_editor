#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "actionhandler.h"
#include "urdf_editor.h"
#include "property.h"
#include <QLabel>
#include <QStatusBar>
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
    ~MainWindow();
public slots:
    void changeMainstatus(QString path);
    void clear();
private slots:
    void on_actionMoveRotate_toggled(bool arg1);
    void on_actionMoveRotateStep_triggered();
    void changeStep(int value);
private:
    float RStep = 0.1;
    float MStep = 0.5;
    bool setmode = 0;
    QLabel *statusLabel = new QLabel("请新建或打开文件", this);
    QStatusBar *status = new QStatusBar(this);
    Ui::MainWindow *ui;
    ActionHandler *actionHandler = new ActionHandler(this);
    Urdf_editor *urdf_editor;
    Property *pro;
    // property2 *pro2;
    shape_relation *tree;
    QAction *setMode = new QAction("mode",this);
    void setToolBar();
signals:
    void MoveRotate(bool mode);
};
#endif // MAINWINDOW_H

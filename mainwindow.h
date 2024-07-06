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
private:
    QLabel *statusLabel = new QLabel("请新建或打开文件", this);
    QStatusBar *status = new QStatusBar(this);
    Ui::MainWindow *ui;
    ActionHandler *actionHandler = new ActionHandler(this);
    Urdf_editor *urdf_editor;
    Property *pro;
    // property2 *pro2;
    shape_relation *tree;
};
#endif // MAINWINDOW_H

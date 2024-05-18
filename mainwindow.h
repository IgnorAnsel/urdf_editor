#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "actionhandler.h"
#include "urdf_editor.h"
#include "property.h"
#include "geometry_choose.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ActionHandler *actionHandler = new ActionHandler(this);
    Urdf_editor *urdf_editor;
    Property *pro;
    geometry_choose *geometry;

};
#endif // MAINWINDOW_H

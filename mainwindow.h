#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "actionhandler.h"
#include "urdf_editor.h"
#include "property.h"
#include "property2.h"
#include "geometry_choose.h"
#include "shape_relation.h"
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
    property2 *pro2;
    geometry_choose *geometry;
    shape_relation *tree;

};
#endif // MAINWINDOW_H

#ifndef GEOMETRY_CHOOSE_H
#define GEOMETRY_CHOOSE_H

#include <QWidget>
#include "urdf_editor.h"
namespace Ui {
class geometry_choose;
}

class geometry_choose : public QWidget
{
    Q_OBJECT

public:
    explicit geometry_choose(QWidget *parent = nullptr);
    ~geometry_choose();
    void createShape(const QString &shapeType);
signals:
    void shapeCreated(const Shape &newShape); // 定义信号
private slots:
    void on_listWidget_currentTextChanged(const QString &currentText);

private:
    Ui::geometry_choose *ui;
};

#endif // GEOMETRY_CHOOSE_H

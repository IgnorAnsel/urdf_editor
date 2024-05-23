#ifndef PROPERTY_H
#define PROPERTY_H

#include <QWidget>
#include "param_struct.h"
#include "urdf_editor.h"
namespace Ui {
class Property;
}

class Property : public QWidget
{
    Q_OBJECT

public:
    explicit Property(QWidget *parent = nullptr);
    void createShape(const QString &shapeType);
    ~Property();
public slots:
    void updateShapeProperties(const Shape &shape); // 定义槽
signals:
    void shapeCreated(const Shape &newShape); // 定义信号
    void createshape(); // 定义create信号
    void updateshape(const Shape &shape); // 重复创建时更新name用信号
private slots:
    void on_collision_geometry_type_currentTextChanged(const QString &arg1);
    void on_visual_geometry_type_currentTextChanged(const QString &arg1);

    void on_link_name_textChanged(const QString &arg1);

    void on_visual_origin_y_textChanged(const QString &arg1);

    void on_visual_origin_x_editingFinished();

    void on_visual_origin_z_editingFinished();

    void on_pushButton_clicked();

    void on_listWidget_currentTextChanged(const QString &currentText);

private:
    URDFLink link;
    Ui::Property *ui;
    Shape currentShape;
    int num = 0;

};

#endif // PROPERTY_H

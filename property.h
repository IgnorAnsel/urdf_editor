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
    ~Property();
public slots:
    void updateShapeProperties(const Shape &shape); // 定义槽
private slots:
    void on_collision_geometry_type_currentTextChanged(const QString &arg1);
    void on_visual_geometry_type_currentTextChanged(const QString &arg1);

    void on_link_name_textChanged(const QString &arg1);

    void on_visual_origin_y_textChanged(const QString &arg1);

    void on_visual_origin_x_editingFinished();

    void on_visual_origin_z_editingFinished();

    void on_pushButton_clicked();

private:
    URDFLink link;
    Ui::Property *ui;
    Shape currentShape;

};

#endif // PROPERTY_H

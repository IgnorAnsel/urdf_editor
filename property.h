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
    void updateShape(int index);
    ~Property();
public slots:
    void updateShapeProperties(const Shape &shape); // 定义槽
    void receiveindex(int index);
signals:
    void shapeCreated(const Shape &newShape); // 定义信号
    void createshape(); // 定义create信号
    void updateshape(const Shape &shape); // 重复创建时更新name用信号
    void updateshapes(); // 选中的物块更改进行更新
private slots:
    void on_collision_geometry_type_currentTextChanged(const QString &arg1);
    void on_visual_geometry_type_currentTextChanged(const QString &arg1);

    // visual_origin
    void on_visual_origin_y_editingFinished();
    void on_visual_origin_x_editingFinished();
    void on_visual_origin_z_editingFinished();


    void on_pushButton_clicked();
    void on_listWidget_currentTextChanged(const QString &currentText);

    void on_visual_origin_r_editingFinished();

    void on_visual_origin_p_editingFinished();

    void on_visual_origin_y_2_editingFinished();

    void on_visual_box_l_editingFinished();

    void on_visual_box_w_editingFinished();

    void on_visual_box_h_editingFinished();

    void on_visual_cylinder_radius_editingFinished();

    void on_visual_cylinder_length_editingFinished();

    void on_visual_sphere_radius_editingFinished();

    void on_toolButton_link_clicked(bool checked);

    void on_toolButton_base_clicked(bool checked);

    void on_toolButton_visuals_clicked(bool checked);

    void on_toolButton_collisions_clicked(bool checked);

    void on_toolButton_inertial_clicked(bool checked);

    void on_toolButton_visuals_origin_clicked(bool checked);

    void on_toolButton_visuals_geometry_clicked(bool checked);

    void on_toolButton_mesh_clicked(bool checked);

    void on_toolButton_meterial_clicked(bool checked);

    void on_toolButton_color_clicked(bool checked);

    void on_toolButton_collisions_origin_clicked(bool checked);

    void on_toolButton_collisions_geometry_clicked(bool checked);

    void on_toolButton_inertial_origin_clicked(bool checked);

    void on_toolButton_inertia_matrix_clicked(bool checked);

    void on_link_name_textEdited(const QString &arg1);

    void on_link_name_editingFinished();

private:
    URDFLink link;
    Ui::Property *ui;
    Shape currentShape;
    Shape currentSetlectShape;
    int currentIndex = -1;
    int num = 0;

};

#endif // PROPERTY_H

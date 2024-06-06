#include "property.h"
#include "ui_property.h"
#include "urdf_editor.h"
Property::Property(QWidget *parent) : QWidget(parent),
                                      ui(new Ui::Property)
{
    ui->setupUi(this);
    on_collision_geometry_type_currentTextChanged(ui->collision_geometry_type->currentText());
    on_visual_geometry_type_currentTextChanged(ui->visual_geometry_type->currentText());
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    ui->listWidget->installEventFilter(this);
    ui->widget->hide();
}

void Property::createShape(const QString &shapeType)
{
    Shape newShape;
    if (shapeType == "Box")
    {
        newShape = Shape(Shape::Cube);
        newShape.link.visuals.geometry.box.size = QVector3D(1.0f, 1.0f, 1.0f);
    }
    else if (shapeType == "Sphere")
    {
        newShape = Shape(Shape::Sphere);
        newShape.link.visuals.geometry.sphere.radius = 1.0f;
    }
    else if (shapeType == "Cylinder")
    {
        newShape = Shape(Shape::Cylinder);
        newShape.link.visuals.geometry.cylinder.radius = 1.0f;
        newShape.link.visuals.geometry.cylinder.length = 2.0f;
    }
    // 设置默认属性
    newShape.link.visuals.origin.xyz = QVector3D(0.0f, 0.0f, 0.0f);
    newShape.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    newShape.link.visuals.color = QColor(Qt::white);
    QString link_name = QString("base_").append(QString::number(num));
    newShape.link.name = link_name.toStdString();
    newShape.link.iscreated = false;
    updateShapeProperties(newShape);
}

void Property::updateShape(int index)
{
    shapes[index].link.visuals.origin.xyz.setY(ui->visual_origin_y->text().toFloat());
    shapes[index].link.visuals.origin.xyz.setX(ui->visual_origin_x->text().toFloat());
    shapes[index].link.visuals.origin.xyz.setZ(ui->visual_origin_z->text().toFloat());
    shapes[index].link.visuals.origin.rpy.setX(ui->visual_origin_r->text().toFloat());
    shapes[index].link.visuals.origin.rpy.setY(ui->visual_origin_p->text().toFloat());
    shapes[index].link.visuals.origin.rpy.setZ(ui->visual_origin_y_2->text().toFloat());
    shapes[index].link.visuals.color.setRedF(ui->visual_color_r->text().toFloat());
    shapes[index].link.visuals.color.setGreenF(ui->visual_color_g->text().toFloat());
    shapes[index].link.visuals.color.setBlueF(ui->visual_color_b->text().toFloat());
    // currentShape.link.visuals.color.setAlphaF(ui->visual_color_a->text().toFloat());
    shapes[index].link.visuals.geometry.box.size.setX(ui->visual_box_l->text().toFloat());
    shapes[index].link.visuals.geometry.box.size.setY(ui->visual_box_w->text().toFloat());
    shapes[index].link.visuals.geometry.box.size.setZ(ui->visual_box_h->text().toFloat());
    shapes[index].link.visuals.geometry.sphere.radius = ui->visual_sphere_radius->text().toFloat();
    shapes[index].link.visuals.geometry.cylinder.radius = ui->visual_cylinder_radius->text().toFloat();
    shapes[index].link.visuals.geometry.cylinder.length = ui->visual_cylinder_length->text().toFloat();
    shapes[index].link.name = ui->link_name->text().toStdString();
    // shapes[index].link.iscreated = true;
    updateShapeProperties(shapes[index]);
    emit updateshapes();
    // emit createshape();
}

void Property::updateJoint()
{
    ui->lineEdit_joint_name->setText(QString::fromStdString(currentjoint.name));
    //ui->comboBox_joint_type
    ui->comboBox_parent_link->setCurrentText(QString::fromStdString(currentjoint.parent_link));
    ui->lineEdit_ptco_r->setText(QString::number(currentjoint.parent_to_child_origin.rpy.x()));
    ui->lineEdit_ptco_p->setText(QString::number(currentjoint.parent_to_child_origin.rpy.y()));
    ui->lineEdit_ptco_y->setText(QString::number(currentjoint.parent_to_child_origin.rpy.z()));
    ui->lineEdit_ptco_x->setText(QString::number(currentjoint.parent_to_child_origin.xyz.x()));
    ui->lineEdit_ptco_y_2->setText(QString::number(currentjoint.parent_to_child_origin.xyz.y()));
    ui->lineEdit_ptco_z->setText(QString::number(currentjoint.parent_to_child_origin.xyz.z()));
}

Property::~Property()
{
    delete ui;
}

void Property::updateShapeProperties(const Shape &shape)
{
    currentShape = shape;
    // 更新UI以显示形状的属性
    if (shape.link.iscreated)
        ui->link_name->setText(QString::fromStdString(shape.link.name));
    else
        ui->link_name->clear();
    ui->visual_origin_x->setText(QString::number(shape.link.visuals.origin.xyz.x()));
    ui->visual_origin_y->setText(QString::number(shape.link.visuals.origin.xyz.y()));
    ui->visual_origin_z->setText(QString::number(shape.link.visuals.origin.xyz.z()));

    ui->visual_origin_r->setText(QString::number(shape.link.visuals.origin.rpy.x()));
    ui->visual_origin_p->setText(QString::number(shape.link.visuals.origin.rpy.y()));
    ui->visual_origin_y_2->setText(QString::number(shape.link.visuals.origin.rpy.z()));

    ui->visual_color_r->setText(QString::number(shape.link.visuals.color.redF()));
    ui->visual_color_g->setText(QString::number(shape.link.visuals.color.greenF()));
    ui->visual_color_b->setText(QString::number(shape.link.visuals.color.blueF()));
    // ui->visual_color_a->setText(QString::number(shape.link.visuals.color.alphaF()));

    ui->visual_box_l->setText(QString::number(shape.link.visuals.geometry.box.size.x()));
    ui->visual_box_w->setText(QString::number(shape.link.visuals.geometry.box.size.y()));
    ui->visual_box_h->setText(QString::number(shape.link.visuals.geometry.box.size.z()));

    ui->visual_sphere_radius->setText(QString::number(shape.link.visuals.geometry.sphere.radius));

    ui->visual_cylinder_radius->setText(QString::number(shape.link.visuals.geometry.cylinder.radius));
    ui->visual_cylinder_length->setText(QString::number(shape.link.visuals.geometry.cylinder.length));
    ui->link_name->setPlaceholderText(QString::fromStdString(QString("base_").append(QString::number(num)).toStdString()));
    if (shape.type == Shape::Cube)
    {
        ui->visual_geometry_type->setCurrentText("Box");
        ui->collision_geometry_type->setCurrentText("Box");
    }
    else if (shape.type == Shape::Sphere)
    {
        ui->visual_geometry_type->setCurrentText("Sphere");
        ui->collision_geometry_type->setCurrentText("Sphere");
    }
    else if (shape.type == Shape::Cylinder)
    {
        ui->visual_geometry_type->setCurrentText("Cylinder");
        ui->collision_geometry_type->setCurrentText("Cylinder");
    }
}

void Property::receiveindex(int index)
{
    ui->widget->hide();
    ui->widget_joint->show();
    if (index >= 0)
    {
        shapes[index].link.iscreated = true;
        currentSetlectShape = shapes[index];
        currentIndex = index;
        updateShapeProperties(currentSetlectShape);
        ui->pushButton->hide();
    }
}

void Property::receivejointindex(int index)
{
    ui->widget->show();
    ui->widget_joint->hide();
    ui->pushButton->hide();
    QStringList strList;
    joint_index = index;
    qDebug() << "index:" <<index;
    for(const auto &shape:shapes)
    {
        if(shape.joint.id == joint_index)
        {
            currentjoint = shape.joint;
        }
        else
            strList.append(QString::fromStdString(shape.link.name));
    }
    qDebug() << "zheli:" << currentjoint.name;
    ui->comboBox_parent_link->clear();
    ui->comboBox_parent_link->addItems(strList);
    updateJoint();
}

void Property::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "mousePressEvent in Property";
    if (event->button() == Qt::LeftButton)
    {
        QListWidgetItem *item = ui->listWidget->itemAt(event->pos());
        qDebug() << "Item found: " << (item != nullptr);
        if (item)
        {
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;
            mimeData->setText(item->text());
            drag->setMimeData(mimeData);
            drag->exec(Qt::CopyAction | Qt::MoveAction);
            qDebug() << "Drag started";
        }
    }
    QWidget::mousePressEvent(event); // 调用基类的事件处理函数
}
bool Property::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->listWidget)
    {
        // qDebug() <<event->type();
        if (event->type() == QEvent::ChildRemoved)
        {
            QListWidgetItem *item_choose;
            for (int i = 0; i < ui->listWidget->count(); ++i)
            {
                QListWidgetItem *item = ui->listWidget->item(i);
                if (item->text() == currenttext)
                {
                    item_choose = item; // 找到的匹配项
                }
            }
            if (item_choose)
            {
                currentShape.link.name = ui->link_name->text().toStdString();
                currentShape.id = num;
                // 添加到形状列表
                if (currentShape.link.visuals.geometry.box.size == QVector3D(0.0f, 0.0f, 0.0f) && currentShape.type == Shape::Cube)
                    return 0;
                if (currentShape.link.name.empty())
                {
                    currentShape.link.name = QString("base_").append(QString::number(num)).toStdString();
                    ui->link_name->setPlaceholderText(QString::fromStdString(currentShape.link.name));
                    num++;
                }
                for (const auto &shape : shapes)
                {
                    if (shape.link.name == currentShape.link.name)
                    {
                        QMessageBox::information(NULL, "ERROR", "The name of the link is already used. Please choose a different name.", QMessageBox::Ok);
                        return 0;
                    }
                }
                updateShapeProperties(currentShape);

                emit drapcreate(currentShape);
                QDrag *drag = new QDrag(this);
                QMimeData *mimeData = new QMimeData;
                mimeData->setText(item_choose->text());
                drag->setMimeData(mimeData);
                drag->exec(Qt::CopyAction | Qt::MoveAction);
                qDebug() << "Drag started" << item_choose->text();
                emit updateshapes(); //
                qDebug() << "4";
            }
            return true; // 表示事件已处理
        }
    }
    return QWidget::eventFilter(obj, event); // 传递未处理的事件
}

void Property::on_collision_geometry_type_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Box")
    {
        ui->collision_geometry_cylinder->hide();
        ui->collision_geometry_sphere->hide();
        ui->collision_geometry_box->show();
    }
    else if (arg1 == "Cylinder")
    {
        ui->collision_geometry_box->hide();
        ui->collision_geometry_sphere->hide();
        ui->collision_geometry_cylinder->show();
    }
    else if (arg1 == "Sphere")
    {
        ui->collision_geometry_box->hide();
        ui->collision_geometry_cylinder->hide();
        ui->collision_geometry_sphere->show();
    }
}

void Property::on_visual_geometry_type_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Box")
    {
        ui->visual_geometry_cylinder->hide();
        ui->visual_geometry_sphere->hide();
        ui->visual_geometry_box->show();
    }
    else if (arg1 == "Cylinder")
    {
        ui->visual_geometry_box->hide();
        ui->visual_geometry_sphere->hide();
        ui->visual_geometry_cylinder->show();
    }
    else if (arg1 == "Sphere")
    {
        ui->visual_geometry_box->hide();
        ui->visual_geometry_cylinder->hide();
        ui->visual_geometry_sphere->show();
    }
    //    if(currentIndex>=0)
    //    {
    //        updateShape(currentIndex);
    //    }
}

void Property::on_visual_origin_y_editingFinished()
{
    currentShape.link.visuals.origin.xyz.setY(ui->visual_origin_y->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_origin_x_editingFinished()
{
    currentShape.link.visuals.origin.xyz.setX(ui->visual_origin_x->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_origin_z_editingFinished()
{
    currentShape.link.visuals.origin.xyz.setZ(ui->visual_origin_z->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_pushButton_clicked()
{
    currentShape.link.visuals.origin.xyz.setY(ui->visual_origin_y->text().toFloat());
    currentShape.link.visuals.origin.xyz.setX(ui->visual_origin_x->text().toFloat());
    currentShape.link.visuals.origin.xyz.setZ(ui->visual_origin_z->text().toFloat());
    currentShape.link.visuals.origin.rpy.setX(ui->visual_origin_r->text().toFloat());
    currentShape.link.visuals.origin.rpy.setY(ui->visual_origin_p->text().toFloat());
    currentShape.link.visuals.origin.rpy.setZ(ui->visual_origin_y_2->text().toFloat());
    currentShape.link.visuals.color.setRedF(ui->visual_color_r->text().toFloat());
    currentShape.link.visuals.color.setGreenF(ui->visual_color_g->text().toFloat());
    currentShape.link.visuals.color.setBlueF(ui->visual_color_b->text().toFloat());
    // currentShape.link.visuals.color.setAlphaF(ui->visual_color_a->text().toFloat());
    currentShape.link.visuals.geometry.box.size.setX(ui->visual_box_l->text().toFloat());
    currentShape.link.visuals.geometry.box.size.setY(ui->visual_box_w->text().toFloat());
    currentShape.link.visuals.geometry.box.size.setZ(ui->visual_box_h->text().toFloat());
    currentShape.link.visuals.geometry.sphere.radius = ui->visual_sphere_radius->text().toFloat();
    currentShape.link.visuals.geometry.cylinder.radius = ui->visual_cylinder_radius->text().toFloat();
    currentShape.link.visuals.geometry.cylinder.length = ui->visual_cylinder_length->text().toFloat();
    currentShape.link.name = ui->link_name->text().toStdString();
    currentShape.id = num;
    // 添加到形状列表
    if (currentShape.link.visuals.geometry.box.size == QVector3D(0.0f, 0.0f, 0.0f) && currentShape.type == Shape::Cube)
        return;
    if (currentShape.link.name.empty())
    {
        currentShape.link.name = QString("base_").append(QString::number(num)).toStdString();
        ui->link_name->setPlaceholderText(QString::fromStdString(currentShape.link.name));
        num++;
    }
    for (const auto &shape : shapes)
    {
        if (shape.link.name == currentShape.link.name)
        {
            QMessageBox::information(NULL, "ERROR", "The name of the link is already used. Please choose a different name.", QMessageBox::Ok);
            return;
        }
    }
    updateShapeProperties(currentShape);
    shapes.push_back(currentShape);
    emit createshape();
    // emit updateshape(currentShape);
    //    emit changenum();
}

void Property::updateformp()
{
    for(auto &shape:shapes)
    {
        if(shape.joint.id == joint_index)
        {
//            qDebug() << "id:" << joint_index
//                     << "shape:"<< shape.joint.name;
            shape.joint = currentjoint;
            break;
        }
        else;
    }
    emit updateshapes();
}

void Property::on_listWidget_currentTextChanged(const QString &currentText)
{
    ui->pushButton->show();
    currenttext = currentText;
    currentIndex = -1;
    createShape(currentText);
}

void Property::on_visual_origin_r_editingFinished()
{
    currentShape.link.visuals.origin.rpy.setX(ui->visual_origin_r->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_origin_p_editingFinished()
{
    currentShape.link.visuals.origin.rpy.setY(ui->visual_origin_p->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_origin_y_2_editingFinished()
{
    currentShape.link.visuals.origin.rpy.setZ(ui->visual_origin_y->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_box_l_editingFinished()
{
    currentShape.link.visuals.geometry.box.size.setX(ui->visual_box_l->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_box_w_editingFinished()
{
    currentShape.link.visuals.geometry.box.size.setY(ui->visual_box_w->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_box_h_editingFinished()
{
    currentShape.link.visuals.geometry.box.size.setZ(ui->visual_box_h->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_cylinder_radius_editingFinished()
{
    currentShape.link.visuals.geometry.cylinder.radius = (ui->visual_cylinder_radius->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_cylinder_length_editingFinished()
{
    currentShape.link.visuals.geometry.cylinder.length = (ui->visual_cylinder_length->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_visual_sphere_radius_editingFinished()
{
    currentShape.link.visuals.geometry.sphere.radius = (ui->visual_sphere_radius->text().toFloat());
    if (currentIndex >= 0)
        updateShape(currentIndex);
}

void Property::on_toolButton_link_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_link->setText("+");
        ui->widget_link->hide();
    }
    else
    {
        ui->toolButton_link->setText("-");
        ui->widget_link->show();
    }
}

void Property::on_toolButton_base_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_base->setText("+");
        ui->widget_base->hide();
    }
    else
    {
        ui->toolButton_base->setText("-");
        ui->widget_base->show();
    }
}

void Property::on_toolButton_visuals_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_visuals->setText("+");
        ui->widget_visuals->hide();
    }
    else
    {
        ui->toolButton_visuals->setText("-");
        ui->widget_visuals->show();
    }
}

void Property::on_toolButton_collisions_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_collisions->setText("+");
        ui->widget_collisions->hide();
    }
    else
    {
        ui->toolButton_collisions->setText("-");
        ui->widget_collisions->show();
    }
}

void Property::on_toolButton_inertial_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_inertial->setText("+");
        ui->widget_inertial->hide();
    }
    else
    {
        ui->toolButton_inertial->setText("-");
        ui->widget_inertial->show();
    }
}

void Property::on_toolButton_visuals_origin_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_visuals_origin->setText("+");
        ui->widget_visuals_origin->hide();
    }
    else
    {
        ui->toolButton_visuals_origin->setText("-");
        ui->widget_visuals_origin->show();
    }
}

void Property::on_toolButton_visuals_geometry_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_visuals_geometry->setText("+");
        ui->widget_visuals_geometry->hide();
    }
    else
    {
        ui->toolButton_visuals_geometry->setText("-");
        ui->widget_visuals_geometry->show();
    }
}

void Property::on_toolButton_mesh_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_mesh->setText("+");
        ui->widget_mesh->hide();
    }
    else
    {
        ui->toolButton_mesh->setText("-");
        ui->widget_mesh->show();
    }
}

void Property::on_toolButton_meterial_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_meterial->setText("+");
        ui->widget_meterial->hide();
    }
    else
    {
        ui->toolButton_meterial->setText("-");
        ui->widget_meterial->show();
    }
}

void Property::on_toolButton_color_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_color->setText("+");
        ui->widget_color->hide();
    }
    else
    {
        ui->toolButton_color->setText("-");
        ui->widget_color->show();
    }
}

void Property::on_toolButton_collisions_origin_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_collisions_origin->setText("+");
        ui->widget_collisions_origin->hide();
    }
    else
    {
        ui->toolButton_collisions_origin->setText("-");
        ui->widget_collisions_origin->show();
    }
}

void Property::on_toolButton_collisions_geometry_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_collisions_geometry->setText("+");
        ui->widget_collisions_geometry->hide();
    }
    else
    {
        ui->toolButton_collisions_geometry->setText("-");
        ui->widget_collisions_geometry->show();
    }
}

void Property::on_toolButton_inertial_origin_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_inertial_origin->setText("+");
        ui->widget_inertial_origin->hide();
    }
    else
    {
        ui->toolButton_inertial_origin->setText("-");
        ui->widget_inertial_origin->show();
    }
}

void Property::on_toolButton_inertia_matrix_clicked(bool checked)
{
    if (checked)
    {
        ui->toolButton_inertia_matrix->setText("+");
        ui->widget_inertia_matrix->hide();
    }
    else
    {
        ui->toolButton_inertia_matrix->setText("-");
        ui->widget_inertia_matrix->show();
    }
}

void Property::on_link_name_editingFinished()
{
    currentShape.link.name = ui->link_name->text().toStdString();
    if (currentIndex >= 0)
    {
        updateShape(currentIndex);
    }
}

void Property::on_lineEdit_ptco_r_editingFinished()
{
    currentjoint.parent_to_child_origin.rpy.setX(ui->lineEdit_ptco_r->text().toFloat());
    updateformp();
}

void Property::on_lineEdit_ptco_p_editingFinished()
{
    currentjoint.parent_to_child_origin.rpy.setY(ui->lineEdit_ptco_p->text().toFloat());
    updateformp();
}

void Property::on_lineEdit_ptco_y_editingFinished()
{
    currentjoint.parent_to_child_origin.rpy.setZ(ui->lineEdit_ptco_y->text().toFloat());
    updateformp();
}


void Property::on_lineEdit_ptco_x_editingFinished()
{
    currentjoint.parent_to_child_origin.xyz.setX(ui->lineEdit_ptco_x->text().toFloat());
    updateformp();
}


void Property::on_lineEdit_ptco_y_2_editingFinished()
{
    currentjoint.parent_to_child_origin.xyz.setY(ui->lineEdit_ptco_y_2->text().toFloat());
    updateformp();
}


void Property::on_lineEdit_ptco_z_editingFinished()
{
    currentjoint.parent_to_child_origin.xyz.setZ(ui->lineEdit_ptco_z->text().toFloat());
    updateformp();
}


void Property::on_comboBox_joint_type_currentTextChanged(const QString &arg1)
{
    currentjoint.type = arg1.toStdString();
    updateformp();
}


void Property::on_lineEdit_joint_name_editingFinished()
{
    currentjoint.name = ui->lineEdit_joint_name->text().toStdString();
    currentjoint.joint_name_manset = true;
    updateformp();
    emit update_items();
}


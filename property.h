#ifndef PROPERTY_H
#define PROPERTY_H

#include <QWidget>
#include "param_struct.h"
namespace Ui {
class Property;
}

class Property : public QWidget
{
    Q_OBJECT

public:
    explicit Property(QWidget *parent = nullptr);
    ~Property();

private slots:
    void on_collision_geometry_type_currentTextChanged(const QString &arg1);
    void on_visual_geometry_type_currentTextChanged(const QString &arg1);

private:
    URDFLink link;
    Ui::Property *ui;
};

#endif // PROPERTY_H

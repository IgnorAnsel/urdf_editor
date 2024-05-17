#ifndef PROPERTY_H
#define PROPERTY_H

#include <QWidget>
#include "param_struct.h"
namespace Ui {
class property;
}

class property : public QWidget
{
    Q_OBJECT

public:
    explicit property(QWidget *parent = nullptr);
    ~property();

private:
    URDFLink link;
    URDFJoint joint_property;
    int type; // 0为link，1为joint
    Ui::property *ui;
};

#endif // PROPERTY_H

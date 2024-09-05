#ifndef SHAPE_H
#define SHAPE_H

#include <QObject>
#include <QVector3D>
#include <QColor>
#include "param_struct.h"

class Shape
{
public:
    enum Type
    {
        Cube,
        Sphere,
        Cylinder,
        // 其他类型...
    };
    Shape(Type type) : type(type)
    {
    }
    Shape() : type(Cube) {}
    int id;
    bool isjointset;
    Type type;
    URDFLink link;
    URDFJoint joint;
    bool isSelected;
//    QVector3D getMinBounds() const;
//    QVector3D getMaxBounds() const;
private:
};

#endif // SHAPE_H

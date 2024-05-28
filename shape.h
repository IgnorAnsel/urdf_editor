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
    Type type;
    URDFLink link;

private:
};

#endif // SHAPE_H

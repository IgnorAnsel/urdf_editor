#include "shapemanager.h"

ShapeManager &ShapeManager::getInstance()
{
    static ShapeManager instance;  // 局部静态实例，保证在整个程序生命周期中只创建一次
    return instance;
}

void ShapeManager::addShape(const Shape &shape)
{
    shapes.push_back(shape);
}

void ShapeManager::removeShape(int index)
{
    if (index >= 0 && index < shapes.size()) {
        shapes.erase(shapes.begin() + index);
    }
}

std::vector<Shape> ShapeManager::getShapes() const
{
    return shapes;
}

std::vector<Shape> &ShapeManager::getShapes()
{
    return shapes;
}

const Shape &ShapeManager::getShape(int index) const
{
    if (index >= 0 && index < shapes.size()) {
        return shapes[index];
    }
    throw std::out_of_range("Shape index is out of range");
}

Shape &ShapeManager::getShape(int index)
{
    if (index >= 0 && index < shapes.size()) {
        return shapes[index];
    }
    throw std::out_of_range("Shape index is out of range");
}

void ShapeManager::updateShape(int index, const Shape &newShape)
{
    if (index >= 0 && index < shapes.size()) {
        shapes[index] = newShape;
    } else {
        throw std::out_of_range("Shape index is out of range");
    }
}

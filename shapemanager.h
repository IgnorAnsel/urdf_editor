#ifndef SHAPEMANAGER_H
#define SHAPEMANAGER_H
#include "shape.h"
#include <QOpenGLShaderProgram>
#include "camera.h"
class ShapeManager {
public:
    // 静态方法，返回唯一的实例
    static ShapeManager& getInstance() {
        static ShapeManager instance;  // 局部静态实例，保证在整个程序生命周期中只创建一次
        return instance;
    }

    // 禁用拷贝构造和赋值操作符，防止复制单例对象
    ShapeManager(const ShapeManager&) = delete;
    ShapeManager& operator=(const ShapeManager&) = delete;

    // 添加其他方法和成员变量
    void addShape(const Shape& shape) {
        shapes.push_back(shape);
    }

    void removeShape(int index) {
        if (index >= 0 && index < shapes.size()) {
            shapes.erase(shapes.begin() + index);
        }
    }

    std::vector<Shape> getShapes() const {
        return shapes;
    }
    std::vector<Shape>& getShapes() {
        return shapes;
    }
    // 通过索引获取某个形状（常量引用）
    const Shape& getShape(int index) const {
        if (index >= 0 && index < shapes.size()) {
            return shapes[index];
        }
        throw std::out_of_range("Shape index is out of range");
    }

    // 通过索引获取某个形状（非常量引用）
    Shape& getShape(int index) {
        if (index >= 0 && index < shapes.size()) {
            return shapes[index];
        }
        throw std::out_of_range("Shape index is out of range");
    }

    // 通过索引更新某个形状
    void updateShape(int index, const Shape& newShape) {
        if (index >= 0 && index < shapes.size()) {
            shapes[index] = newShape;
        } else {
            throw std::out_of_range("Shape index is out of range");
        }
    }
private:
    // 私有构造函数，确保外部无法创建实例
    ShapeManager() {}

    // 形状列表
    std::vector<Shape> shapes;
};



#endif // SHAPEMANAGER_H

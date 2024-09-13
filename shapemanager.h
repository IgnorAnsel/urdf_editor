#ifndef SHAPEMANAGER_H
#define SHAPEMANAGER_H
#include "shape.h"
#include <QOpenGLShaderProgram>
#include "camera.h"
class ShapeManager {
public:
    // 静态方法，返回唯一的实例
    static ShapeManager& getInstance();
    // 禁用拷贝构造和赋值操作符，防止复制单例对象
    ShapeManager(const ShapeManager&) = delete;
    ShapeManager& operator=(const ShapeManager&) = delete;


    void addShape(const Shape& shape);
    void removeShape(int index);

    std::vector<Shape> getShapes() const;
    std::vector<Shape>& getShapes();
    // 通过索引获取某个形状（常量引用）
    const Shape& getShape(int index) const;

    // 通过索引获取某个形状（非常量引用）
    Shape& getShape(int index);

    // 通过索引更新某个形状
    void updateShape(int index, const Shape& newShape);
private:
    // 私有构造函数，确保外部无法创建实例
    ShapeManager() {}
    // 形状列表
    std::vector<Shape> shapes;
};



#endif // SHAPEMANAGER_H

#ifndef SHAPEMANAGER_H
#define SHAPEMANAGER_H
#include "shape.h"
#include <QOpenGLShaderProgram>
#include "camera.h"
class ShapeManager {
public:
    ShapeManager();

    void addShape(const Shape& shape) {
        shapes.push_back(shape);
    }

    void removeShape(int index) {
        if (index >= 0 && index < shapes.size()) {
            shapes.erase(shapes.begin() + index);
        }
    }

    Shape& getShape(int index) {
        return shapes.at(index);
    }

    std::vector<Shape>& getAllShapes() {
        return shapes;
    }

    void renderShapes(QOpenGLShaderProgram &shaderProgram, QMatrix4x4 &viewMatrix, QMatrix4x4 &projectionMatrix, Camera &camera) {
        for (auto& shape : shapes) {
            //renderShape(shape, shaderProgram, viewMatrix, projectionMatrix, camera);
        }
    }

private:
    std::vector<Shape> shapes;
    void applyTransform(QMatrix4x4 &modelMatrix, const QVector3D &position, const QQuaternion &rotation);
    void applyTransform(QMatrix4x4 &matrix, const QVector3D &translation, const QVector3D &rotation);
};


#endif // SHAPEMANAGER_H

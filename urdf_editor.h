#ifndef URDF_EDITOR_H
#define URDF_EDITOR_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include "shape.h"
#include "param_struct.h"
class Urdf_editor : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit Urdf_editor(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void drawCube(const Shape &shape);
    void drawSphere(const Shape &shape);
    void drawCylinder(const Shape &shape);
private:
    void renderShape(const Shape &shape);
    QMatrix4x4 viewMatrix;
    QPoint lastMousePos;
    float zoomFactor;
    float rotationAngleX;
    float rotationAngleY;
    Shape cube; // 将cube定义为类的成员变量
    Shape sphere;
    Shape cylinder;
};

#endif // URDF_EDITOR_H

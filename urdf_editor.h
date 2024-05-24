#ifndef URDF_EDITOR_H
#define URDF_EDITOR_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include "shape.h"
#include "param_struct.h"
extern std::vector<Shape> shapes;
class Urdf_editor : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    explicit Urdf_editor(QWidget *parent = nullptr);
public slots:
    void updateShape(); // 定义槽
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
    int selectedShapeIndex = -1; // -1 表示没有选中任何形状
    void renderShape(const Shape &shape);
    QMatrix4x4 viewMatrix;
    QPoint lastMousePos;
    float zoomFactor;
    float rotationAngleX;
    float rotationAngleY;
    Shape cube; // 将cube定义为类的成员变量
    Shape sphere;
    Shape cylinder;
    GLdouble projectionMatrix[16];
    GLdouble modelviewMatrix[16];
    GLint viewport[4];
};

#endif // URDF_EDITOR_H

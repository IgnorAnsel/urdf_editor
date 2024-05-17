#ifndef URDF_EDITOR_H
#define URDF_EDITOR_H

#include <QWidget>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLFunctions_3_3_Core>
namespace Ui {
class Urdf_editor;
}

class Urdf_editor : public QOpenGLWidget , protected QOpenGLFunctions
{
public:
    Urdf_editor(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPoint lastMousePos;
    QMatrix4x4 viewMatrix;
    float zoomFactor = 1.0f;
    float rotationAngleX = 0.0f;
    float rotationAngleY = 0.0f;
};

#endif // URDF_EDITOR_H

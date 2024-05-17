#include "urdf_editor.h"
#include "ui_urdf_editor.h"
#include <iostream>
#include <qdebug.h>
Urdf_editor::Urdf_editor(QWidget *parent) : QOpenGLWidget(parent) {
    setMinimumSize(640, 480);
}

void Urdf_editor::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Urdf_editor::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);

    // 创建透视投影矩阵
    QMatrix4x4 projection;
    projection.perspective(45.0, double(w) / double(h), 0.1, 100.0);

    // 应用投影矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(projection.constData());

    // 切换回模型视图矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void Urdf_editor::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 更新视图矩阵
    QMatrix4x4 transform;
    transform.translate(0.0f, 0.0f, -zoomFactor * 15.0f);
    transform.rotate(rotationAngleX, 1.0f, 0.0f, 0.0f);
    transform.rotate(rotationAngleY, 0.0f, 1.0f, 0.0f);

    viewMatrix = transform;
    glLoadMatrixf(viewMatrix.constData());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 设置相机位置和目标点
    QVector3D eye(3.0f, 4.0f, 5.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector3D up(0.0f, 1.0f, 0.0f);

    // 创建相机变换矩阵
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(eye, center, up);

    // 应用相机变换
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix.constData());

    // 绘制坐标轴
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(10.0, 0.0, 0.0); // X 轴
    glColor3f(0.0, 1.0, 0.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(0.0, 10.0, 0.0); // Y 轴
    glColor3f(0.0, 0.0, 1.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(0.0, 0.0, 10.0); // Z 轴
    glEnd();
}
void Urdf_editor::mousePressEvent(QMouseEvent *event) {
    lastMousePos = event->pos();
    event->accept();
}

void Urdf_editor::mouseMoveEvent(QMouseEvent *event) {
    int dx = event->x() - lastMousePos.x();
    int dy = event->y() - lastMousePos.y();

    if (event->buttons() & Qt::LeftButton) {
        rotationAngleX += dy;
        rotationAngleY += dx;
    }

    lastMousePos = event->pos();
    update();
    event->accept();

}

void Urdf_editor::mouseReleaseEvent(QMouseEvent *event) {
    // 这里可以添加需要的功能，如结束拖拽效果等
}

void Urdf_editor::wheelEvent(QWheelEvent *event) {
    zoomFactor += event->angleDelta().y() / 1200.0f;
    zoomFactor = std::max(0.1f, std::min(zoomFactor, 10.0f));
    update();
    event->accept();
}









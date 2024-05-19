#include "urdf_editor.h"
#include "ui_urdf_editor.h"
#include <iostream>
#include <qdebug.h>
Urdf_editor::Urdf_editor(QWidget *parent) : QOpenGLWidget(parent), cube(Shape::Cube) {
    setMinimumSize(640, 480);
    zoomFactor = 1.0f;
    rotationAngleX = 0.0f;
    rotationAngleY = 0.0f;

    // 初始化立方体对象并设置属性
    cube.link.visuals.origin.xyz = QVector3D(0.0f, 0.0f, 0.0f);
    //cube.scale = QVector3D(1.0f, 1.0f, 1.0f);
    cube.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    cube.link.visuals.color = QColor(Qt::red);
    cube.link.visuals.geometry.box.size = QVector3D(10.0f,1.0f,1.0f);

    sphere.link.visuals.origin.xyz = QVector3D(1.0f,0.0f,0.0f);
    sphere.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    sphere.link.visuals.geometry.sphere.radius = 5;

}

void Urdf_editor::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
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
}

void Urdf_editor::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 更新视图矩阵
    QMatrix4x4 transform;
    transform.translate(0.0f, 0.0f, -zoomFactor * 15.0f);
    transform.rotate(rotationAngleX, 1.0f, 0.0f, 0.0f);
    transform.rotate(rotationAngleY, 0.0f, 1.0f, 0.0f);

    viewMatrix = transform;

    // 设置相机位置和目标点
    QVector3D eye(3.0f, 4.0f, 5.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector3D up(0.0f, 1.0f, 0.0f);

    // 创建相机变换矩阵
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(eye, center, up);

    // 应用相机变换和视图变换
    viewMatrix *= transform;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(viewMatrix.constData());

    // 绘制坐标轴
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(100.0, 0.0, 0.0); // X 轴
    glColor3f(0.0, 1.0, 0.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(0.0, 100.0, 0.0); // Y 轴
    glColor3f(0.0, 0.0, 1.0); glVertex3f(0.0, 0.0, 0.0); glVertex3f(0.0, 0.0, 100.0); // Z 轴
    glEnd();

    // 根据形状对象的类型来绘制
    renderShape(cube);
}

void Urdf_editor::drawCube(const Shape &shape) {
    // 设置颜色
    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

    QVector3D size = shape.link.visuals.geometry.box.size;

    glBegin(GL_QUADS);

    // Front face
    glVertex3f(-size.x() / 2, -size.y() / 2, size.z() / 2);
    glVertex3f(size.x() / 2, -size.y() / 2, size.z() / 2);
    glVertex3f(size.x() / 2, size.y() / 2, size.z() / 2);
    glVertex3f(-size.x() / 2, size.y() / 2, size.z() / 2);

    // Back face
    glVertex3f(-size.x() / 2, -size.y() / 2, -size.z() / 2);
    glVertex3f(-size.x() / 2, size.y() / 2, -size.z() / 2);
    glVertex3f(size.x() / 2, size.y() / 2, -size.z() / 2);
    glVertex3f(size.x() / 2, -size.y() / 2, -size.z() / 2);

    // Top face
    glVertex3f(-size.x() / 2, size.y() / 2, -size.z() / 2);
    glVertex3f(-size.x() / 2, size.y() / 2, size.z() / 2);
    glVertex3f(size.x() / 2, size.y() / 2, size.z() / 2);
    glVertex3f(size.x() / 2, size.y() / 2, -size.z() / 2);

    // Bottom face
    glVertex3f(-size.x() / 2, -size.y() / 2, -size.z() / 2);
    glVertex3f(size.x() / 2, -size.y() / 2, -size.z() / 2);
    glVertex3f(size.x() / 2, -size.y() / 2, size.z() / 2);
    glVertex3f(-size.x() / 2, -size.y() / 2, size.z() / 2);

    // Right face
    glVertex3f(size.x() / 2, -size.y() / 2, -size.z() / 2);
    glVertex3f(size.x() / 2, size.y() / 2, -size.z() / 2);
    glVertex3f(size.x() / 2, size.y() / 2, size.z() / 2);
    glVertex3f(size.x() / 2, -size.y() / 2, size.z() / 2);

    // Left face
    glVertex3f(-size.x() / 2, -size.y() / 2, -size.z() / 2);
    glVertex3f(-size.x() / 2, -size.y() / 2, size.z() / 2);
    glVertex3f(-size.x() / 2, size.y() / 2, size.z() / 2);
    glVertex3f(-size.x() / 2, size.y() / 2, -size.z() / 2);

    glEnd();
}
void Urdf_editor::drawSphere(const Shape &shape) {
    // 设置颜色
    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

    double radius = shape.link.visuals.geometry.sphere.radius;

    const int slices = 30;
    const int stacks = 30;

    GLUquadric* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);

    gluSphere(quadric, radius, slices, stacks);

    gluDeleteQuadric(quadric);
}

void Urdf_editor::drawCylinder(const Shape &shape) {
    // 设置颜色
    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

    double radius = shape.link.visuals.geometry.cylinder.radius;
    double length = shape.link.visuals.geometry.cylinder.length;

    const int slices = 30;
    const int stacks = 1;

    GLUquadric* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);

    gluCylinder(quadric, radius, radius, length, slices, stacks);

    gluDeleteQuadric(quadric);
}


void Urdf_editor::renderShape(const Shape &shape) {
    // 设置几何变换
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(shape.link.visuals.origin.xyz);
    modelMatrix.scale(QVector3D(1.0f, 1.0f, 1.0f));
    modelMatrix.rotate(shape.link.visuals.origin.rpy.x(), 1.0f, 0.0f, 0.0f);
    modelMatrix.rotate(shape.link.visuals.origin.rpy.y(), 0.0f, 1.0f, 0.0f);
    modelMatrix.rotate(shape.link.visuals.origin.rpy.z(), 0.0f, 0.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(modelMatrix.constData());

    // 绘制形状
    if (shape.type == Shape::Cube) {
        drawCube(shape);
    }

    glPopMatrix();
}

void Urdf_editor::mousePressEvent(QMouseEvent *event) {
    lastMousePos = event->pos();
}

void Urdf_editor::mouseMoveEvent(QMouseEvent *event) {
    float dx = float(event->x() - lastMousePos.x()) / width();
    float dy = float(event->y() - lastMousePos.y()) / height();

    if (event->buttons() & Qt::LeftButton) {
        rotationAngleX += dy * 180;
        rotationAngleY += dx * 180;
    } else if (event->buttons() & Qt::RightButton) {
        rotationAngleX += dy * 180;
        rotationAngleY += dx * 180;
    }

    lastMousePos = event->pos();
    update();
}

void Urdf_editor::mouseReleaseEvent(QMouseEvent *event) {
    // 处理鼠标释放事件
}

void Urdf_editor::wheelEvent(QWheelEvent *event) {
    zoomFactor -= event->angleDelta().y() / 1200.0f;
    if (zoomFactor < 0.1f)
        zoomFactor = 0.1f;
    update();
}

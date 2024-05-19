#include "urdf_editor.h"
#include "ui_urdf_editor.h"
#include <iostream>
#include <qdebug.h>
#include <cmath>
#include <GL/glu.h>
std::vector<Shape> shapes;

Urdf_editor::Urdf_editor(QWidget *parent) : QOpenGLWidget(parent), cube(Shape::Cube), sphere(Shape::Sphere),cylinder(Shape::Cylinder) {
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
    sphere.link.visuals.geometry.sphere.radius = 1;
    sphere.link.visuals.color = QColor(Qt::blue);

    cylinder.link.visuals.origin.xyz = QVector3D(1.0f,0.0f,0.0f);
    cylinder.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    cylinder.link.visuals.geometry.cylinder.radius = 1;
    cylinder.link.visuals.geometry.cylinder.length = 2;
    cylinder.link.visuals.color = QColor(Qt::green);

    //shapes.push_back(cube);
    //shapes.push_back(sphere);
    //shapes.push_back(cylinder);
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

    // 渲染列表中的所有形状对象
    for (size_t i = 0; i < shapes.size(); ++i) {
        if (i == selectedShapeIndex) {
            // 高亮显示选中的形状
            glColor3f(1.0, 1.0, 0.0); // 使用黄色显示选中的形状
        }
        renderShape(shapes[i]);
        if (i == selectedShapeIndex) {
            // 恢复正常颜色
            glColor3f(1.0, 1.0, 1.0); // 恢复为白色
        }
    }

    // 获取投影矩阵、模型视图矩阵和视口
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
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

    for (int i = 0; i <= stacks; ++i) {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / stacks);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / stacks);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            double lng = 2 * M_PI * (double)(j - 1) / slices;
            double x = cos(lng);
            double y = sin(lng);

            glNormal3d(x * zr0, y * zr0, z0);
            glVertex3d(x * zr0 * radius, y * zr0 * radius, z0 * radius);

            glNormal3d(x * zr1, y * zr1, z1);
            glVertex3d(x * zr1 * radius, y * zr1 * radius, z1 * radius);
        }
        glEnd();
    }
}


void Urdf_editor::drawCylinder(const Shape &shape) {
    // 设置颜色
    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

    double radius = shape.link.visuals.geometry.cylinder.radius;
    double height = shape.link.visuals.geometry.cylinder.length;
    const int slices = 30;

    // 绘制圆柱体的圆面
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= slices; ++i) {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, height);
    for (int i = 0; i <= slices; ++i) {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, height);
    }
    glEnd();

    // 绘制圆柱体的侧面
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glNormal3f(x, y, 0.0f);
        glVertex3f(x, y, 0.0f);
        glVertex3f(x, y, height);
    }
    glEnd();
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
    } else if (shape.type == Shape::Sphere) {
        drawSphere(shape);
    } else if (shape.type == Shape::Cylinder) {
        drawCylinder(shape);
    }

    glPopMatrix();
}

// 在鼠标点击事件中检测是否点击了某个形状
void Urdf_editor::mousePressEvent(QMouseEvent *event) {
    qDebug() << "Mouse pressed at" << event->pos();
    lastMousePos = event->pos();
    float minDistance = 1000.0f;
    int closestShapeIndex = -1;

    // 获取鼠标点击位置
    GLint winX = event->pos().x();
    GLint winY = viewport[3] - event->pos().y(); // 注意 Y 轴的方向
    GLfloat winZ;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    // 将窗口坐标转换为世界坐标
    GLdouble worldX, worldY, worldZ;
    gluUnProject(winX, winY, winZ, modelviewMatrix, projectionMatrix, viewport, &worldX, &worldY, &worldZ);
    QVector3D clickPos(worldX, worldY, worldZ);

    for (size_t i = 0; i < shapes.size(); ++i) {
        QVector3D shapePos = shapes[i].link.visuals.origin.xyz;
        qDebug() << "Checking shape at" << shapePos << "with index" << i;
        float distance = (shapePos - clickPos).length();
        qDebug() << "Distance to shape" << distance;
        if (distance < minDistance) {
            minDistance = distance;
            closestShapeIndex = i;
        }
    }

    if (minDistance < 50.0f) { // 如果距离小于一定阈值，认为点击到了形状
        selectedShapeIndex = closestShapeIndex;
        qDebug() << "Shape selected:" << selectedShapeIndex;
    } else {
        selectedShapeIndex = -1;
        qDebug() << "No shape selected";
    }
    update();
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

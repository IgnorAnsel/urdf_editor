#include "urdf_editor.h"
#include "ui_urdf_editor.h"
#include <iostream>
#include <qdebug.h>
#include <cmath>
#include <GL/glu.h>
std::vector<Shape> shapes;

Urdf_editor::Urdf_editor(QWidget *parent) : QOpenGLWidget(parent), cube(Shape::Cube), sphere(Shape::Sphere), cylinder(Shape::Cylinder)
{
    setMinimumSize(640, 480);
    zoomFactor = 1.0f;
    rotationAngleX = 0.0f;
    rotationAngleY = 0.0f;
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true); // 启用拖放功能
}

void Urdf_editor::reset()
{
    shapes.clear();
    joints.clear();
    selectedShapeIndex = -1;
    lastselectedShapeIndex = -1;
}

void Urdf_editor::receiveShapeKind(int kind)
{
    shapekind = kind;
}

void Urdf_editor::updateWHLRStep(float cube_W, float cube_H, float cube_L, float cyliner_H, float cyliner_R, float sphere_R)
{
    qDebug()<<"dasds";
    Cube_W = cube_W;
    Cube_H = cube_H;
    Cube_L = cube_L;
    Cyliner_H = cyliner_H;
    Cyliner_R = cyliner_R;
    Sphere_R = sphere_R;
}

void Urdf_editor::changeStep(int value)
{
    if(MoveRotateMode)
        Rotatestep = value/200.0;
    else
        Movestep = value/200.0;
}

void Urdf_editor::ChangeMoveRotate(bool mode)
{
    MoveRotateMode = mode;
}

void Urdf_editor::updateShape()
{
    // selectedShapeIndex = shapes.size()-1;
    update(); // 请求重新绘制窗口
}
void Urdf_editor::receiveIndex(int index)
{
    selectedShapeIndex = index;
    update(); // 请求重新绘制窗口
}

void Urdf_editor::dropCreate(const Shape &shape)
{
    currentShape = shape;
}

void Urdf_editor::updateJoint()
{
    for(const auto &shape:shapes)
    {
        renderShape(shape);
    }
}

void Urdf_editor::set_set_selectedShapeIndex_f1()
{
    selectedShapeIndex = -1;
    shapes[lastselectedShapeIndex].link.visuals.color = precolor;
    qDebug()<<"pre:"<<precolor;
    update();
}
void Urdf_editor::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // 设置背景颜色为灰色
}
void Urdf_editor::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    // 创建透视投影矩阵
    QMatrix4x4 projection;
    projection.perspective(45.0, double(w) / double(h), 0.1, 100.0);

    // 应用投影矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(projection.constData());
}

void Urdf_editor::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 更新视图矩阵
    QMatrix4x4 transform;
    transform.translate(0.0f, 0.0f, -zoomFactor * 15.0f);
    transform.rotate(rotationAngleX, 1.0f, 0.0f, 0.0f);
    transform.rotate(rotationAngleY, 0.0f, 1.0f, 0.0f);

    // 设置相机位置和目标点
    //    QVector3D eye(0.0f, 0.0f, zoomFactor * 10.0f); // 相机位置
    //    QVector3D center(0.0f, 0.0f, 0.0f); // 看向的中心点
    //    QVector3D up(0.0f, 1.0f, 0.0f); // 上方向

    eye = QVector3D(0.0f, 0.0f, zoomFactor * 10.0f); // 相机位置
    QVector3D center(0.0f, 0.0f, 0.0f);            // 看向的中心点，使相机朝向 y 轴正方向
    QVector3D up(-1.0f, 0.0f, 0.0f);               // 上方向，使 x 轴朝向相机

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
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(100.0, 0.0, 0.0); // X 轴
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 100.0, 0.0); // Y 轴
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 100.0); // Z 轴
    // 绘制平面
    drawPlane(10, 10, 1);

    glEnd();
    // 渲染列表中的所有形状对象
    for (size_t i = 0; i < shapes.size(); ++i)
    {
        // 如果选中的形状发生变化，更新颜色
        if (selectedShapeIndex >= 0)
        {
            // 恢复之前选中的形状的颜色
            shapes[lastselectedShapeIndex].link.visuals.color = precolor;
            // 保存当前选中形状的原始颜色
            precolor = shapes[selectedShapeIndex].link.visuals.color;
            // 高亮显示新选中的形状
            shapes[selectedShapeIndex].link.visuals.color = QColor(Qt::yellow);
            lastselectedShapeIndex = selectedShapeIndex;
            glPushMatrix();
            float localX = shapes[selectedShapeIndex].link.visuals.origin.xyz.x();
            float localY = shapes[selectedShapeIndex].link.visuals.origin.xyz.y();
            float localZ = shapes[selectedShapeIndex].link.visuals.origin.xyz.z();
            float RotateR = shapes[selectedShapeIndex].link.visuals.origin.rpy.x();
            float RotateP = shapes[selectedShapeIndex].link.visuals.origin.rpy.y();
            float RotateY = shapes[selectedShapeIndex].link.visuals.origin.rpy.z();
            if (MoveRotateMode == 0)
            {

                    glPushMatrix();
                    glTranslatef(localX, localY, localZ);
                    // X 轴红色
                    glEnd();
                    drawArrow(0,0,0,2,0,0,Qt::red);
                    glPopMatrix();
                    // Y 轴绿色
                    glPushMatrix();
                    glTranslatef(localX, localY, localZ);
                    glEnd();
                    drawArrow(0,0,0,0,2,0,Qt::green);
                    glPopMatrix();
                    glPushMatrix();
                    glTranslatef(localX, localY, localZ);
                    // Z 轴蓝色
                    glEnd();
                    drawArrow(0,0,0,0,0,2,Qt::blue);
                    glPopMatrix();
            }

            else if(MoveRotateMode==1)
            {
                    glPushMatrix();
                    glTranslatef(localX, localY, localZ);

                    // 创建旋转四元数
                    glm::quat quaternionX = glm::angleAxis(RotateR, glm::vec3(1.0f, 0.0f, 0.0f));
                    glm::quat quaternionY = glm::angleAxis(RotateP, glm::vec3(0.0f, 1.0f, 0.0f));
                    glm::quat quaternionZ = glm::angleAxis(RotateY, glm::vec3(0.0f, 0.0f, 1.0f));

                    // 合并旋转四元数
                    glm::quat combinedQuaternion = quaternionZ * quaternionY * quaternionX;

                    // 将四元数转换为旋转矩阵
                    glm::mat4 rotationMatrix = glm::toMat4(combinedQuaternion);

                    // 将旋转矩阵应用到当前矩阵堆栈

                    // X 轴红色
                    glPushMatrix();

                    //glTranslatef(localX, localY, localZ);
                    glMultMatrixf(glm::value_ptr(rotationMatrix));

                    drawArrow(0, 0, 0, 2, 0, 0, Qt::red);
                    glPopMatrix();

                    // Y 轴绿色
                    glPushMatrix();

                    glTranslatef(localX, localY, localZ);
                    glMultMatrixf(glm::value_ptr(rotationMatrix));

                    drawArrow(0, 0, 0, 0, 2, 0, Qt::green);
                    glPopMatrix();

                    // Z 轴蓝色
                    glPushMatrix();
                    glTranslatef(localX, localY, localZ);
                    glMultMatrixf(glm::value_ptr(rotationMatrix));

                    drawArrow(0, 0, 0, 0, 0, 2, Qt::blue);
                    glPopMatrix();

                    glPopMatrix();

            }
        }
        renderShape(shapes[i]); // 渲染每个形状
    }

    // 获取投影矩阵、模型视图矩阵和视口
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
}

void Urdf_editor::drawCube(const Shape &shape)
{
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
inline double Urdf_editor::radiansToDegrees(double radians) {
    return radians * (180.0 / M_PI);
}
void Urdf_editor::drawSphere(const Shape &shape)
{
    // 设置颜色
    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

    double radius = shape.link.visuals.geometry.sphere.radius;
    const int slices = 30;
    const int stacks = 30;

    for (int i = 0; i <= stacks; ++i)
    {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / stacks);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / stacks);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j)
        {
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

void Urdf_editor::drawCylinder(const Shape &shape)
{
    // 设置颜色
    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

    double radius = shape.link.visuals.geometry.cylinder.radius;
    double height = shape.link.visuals.geometry.cylinder.length;
    const int slices = 30;
    double halfHeight = height / 2.0;

    // 绘制圆柱体的底面
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, -halfHeight);
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, -halfHeight);
    }
    glEnd();

    // 绘制圆柱体的顶面
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, halfHeight);
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, halfHeight);
    }
    glEnd();

    // 绘制圆柱体的侧面
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glNormal3f(x, y, 0.0f);
        glVertex3f(x, y, -halfHeight);
        glVertex3f(x, y, halfHeight);
    }
    glEnd();
}

void Urdf_editor::drawArrow(float x, float y, float z, float dx, float dy, float dz, QColor color)
{
    // 设置箭头颜色
    glColor3f(color.redF(), color.greenF(), color.blueF());
    glLineWidth(6.0f);

    // 计算箭头末端位置
    float length = sqrt(dx * dx + dy * dy + dz * dz);

    // 计算单位方向向量
    float nx = dx / length;
    float ny = dy / length;
    float nz = dz / length;

    // 绘制箭头线段
    glBegin(GL_LINES);
    glVertex3f(x, y, z);
    glVertex3f(x + dx, y + dy, z + dz);
    glEnd();
    // 设置圆锥的变换
    glPushMatrix();
    glTranslatef(x + dx, y + dy, z + dz); // 移动到箭头末端
    // 使圆锥朝向箭头方向
    // 需要找到旋转轴和旋转角度
    float angle = acos(nz) * 180.0 / M_PI; // 计算与z轴的角度
    glRotatef(angle, -ny, nx, 0.0f); // 绕旋转轴旋转
    drawCone(coneHeight, coneRadius, color); // 绘制圆锥
    glPopMatrix();
}




void Urdf_editor::drawCone(float height, float radius, QColor color)
{
    // 设置颜色
    glColor3f(color.redF(), color.greenF(), color.blueF());

    const int slices = 30;
    double halfHeight = height / 2.0;

    // 绘制圆锥体的底面
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, -halfHeight);
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, -halfHeight);
    }
    glEnd();

    // 绘制圆锥体的侧面
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, halfHeight);
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, -halfHeight);
    }
    glEnd();
}

void Urdf_editor::drawPlane(float width, float height, float gridSize)
{
    // 设置平面颜色
    glColor3f(0.7f, 0.7f, 0.7f);

    // 计算平面顶点
    int numGridX = width / gridSize;
    int numGridY = height / gridSize;

    // 绘制网格线
    glBegin(GL_LINES);
    for (int i = -numGridX / 2; i <= numGridX / 2; ++i)
    {
        glVertex3f(i * gridSize, -height / 2, 0.0f);
        glVertex3f(i * gridSize, height / 2, 0.0f);
    }
    for (int j = -numGridY / 2; j <= numGridY / 2; ++j)
    {
        glVertex3f(-width / 2, j * gridSize, 0.0f);
        glVertex3f(width / 2, j * gridSize, 0.0f);
    }
    glEnd();
}


void Urdf_editor::applyTransform(QMatrix4x4 &matrix, const QVector3D &translation, const QVector3D &rotation)
{
    matrix.translate(translation);
    matrix.rotate(qRadiansToDegrees(rotation.z()), 0.0f, 0.0f, 1.0f);
    matrix.rotate(qRadiansToDegrees(rotation.y()), 0.0f, 1.0f, 0.0f);
    matrix.rotate(qRadiansToDegrees(rotation.x()), 1.0f, 0.0f, 0.0f);
}
void Urdf_editor::handleKey_Move(int key)
{
    switch (key)
    {
    case Qt::Key_W:
    {
        shapes[selectedShapeIndex].link.visuals.origin.xyz.setX(shapes[selectedShapeIndex].link.visuals.origin.xyz.x()-Movestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_S:
    {
        shapes[selectedShapeIndex].link.visuals.origin.xyz.setX(shapes[selectedShapeIndex].link.visuals.origin.xyz.x()+Movestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_A:
    {
        shapes[selectedShapeIndex].link.visuals.origin.xyz.setY(shapes[selectedShapeIndex].link.visuals.origin.xyz.y()-Movestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_D:
    {
        shapes[selectedShapeIndex].link.visuals.origin.xyz.setY(shapes[selectedShapeIndex].link.visuals.origin.xyz.y()+Movestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_Up:
    {
        shapes[selectedShapeIndex].link.visuals.origin.xyz.setZ(shapes[selectedShapeIndex].link.visuals.origin.xyz.z()+Movestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_Down:
    {
        shapes[selectedShapeIndex].link.visuals.origin.xyz.setZ(shapes[selectedShapeIndex].link.visuals.origin.xyz.z()-Movestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    }
}

void Urdf_editor::handleKey_Rotate(int key)
{
    switch (key)
    {
    case Qt::Key_W:
    {
        shapes[selectedShapeIndex].link.visuals.origin.rpy.setY(shapes[selectedShapeIndex].link.visuals.origin.rpy.y()-Rotatestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_S:
    {
        shapes[selectedShapeIndex].link.visuals.origin.rpy.setY(shapes[selectedShapeIndex].link.visuals.origin.rpy.y()+Rotatestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_A:
    {
        shapes[selectedShapeIndex].link.visuals.origin.rpy.setX(shapes[selectedShapeIndex].link.visuals.origin.rpy.x()+Rotatestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_D:
    {
        shapes[selectedShapeIndex].link.visuals.origin.rpy.setX(shapes[selectedShapeIndex].link.visuals.origin.rpy.x()-Rotatestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_Up:
    {
        shapes[selectedShapeIndex].link.visuals.origin.rpy.setZ(shapes[selectedShapeIndex].link.visuals.origin.rpy.z()+Rotatestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    case Qt::Key_Down:
    {
        shapes[selectedShapeIndex].link.visuals.origin.rpy.setZ(shapes[selectedShapeIndex].link.visuals.origin.rpy.z()-Rotatestep);
        emit updateIndex(selectedShapeIndex);
        break;
    }
    }
}

void Urdf_editor::handleKey_WHLR_Plus(int key)
{
    if(shapekind==0)
    {
        switch (key)
        {
        case Qt::Key_X:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.box.size.setX(shapes[selectedShapeIndex].link.visuals.geometry.box.size.x()+Cube_L);
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Y:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.box.size.setY(shapes[selectedShapeIndex].link.visuals.geometry.box.size.y()+Cube_W);
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Z:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.box.size.setZ(shapes[selectedShapeIndex].link.visuals.geometry.box.size.z()+Cube_H);
            emit updateIndex(selectedShapeIndex);
            break;
        }
        }
    }
    else if(shapekind==1)
    {
        switch (key)
        {
        case Qt::Key_X:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius = shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius + Cyliner_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Y:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius = shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius + Cyliner_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Z:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length = shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length + Cyliner_H;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        }
    }
    else if(shapekind==2)
    {
        switch (key)
        {
        case Qt::Key_X:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius = shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius + Sphere_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Y:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius = shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius + Sphere_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Z:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius = shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius + Sphere_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        }
    }
}

void Urdf_editor::handleKey_WHLR_Minus(int key)
{
    if(shapekind==0)
    {
        switch (key)
        {
        case Qt::Key_X:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.box.size.setX(shapes[selectedShapeIndex].link.visuals.geometry.box.size.x()-Cube_L);
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Y:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.box.size.setY(shapes[selectedShapeIndex].link.visuals.geometry.box.size.y()-Cube_W);
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Z:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.box.size.setZ(shapes[selectedShapeIndex].link.visuals.geometry.box.size.z()-Cube_H);
            emit updateIndex(selectedShapeIndex);
            break;
        }
        }
    }
    else if(shapekind==1)
    {
        switch (key)
        {
        case Qt::Key_X:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius = shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius - Cyliner_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Y:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius = shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius - Cyliner_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Z:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length = shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length - Cyliner_H;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        }
    }
    else if(shapekind==2)
    {
        switch (key)
        {
        case Qt::Key_X:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius = shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius - Sphere_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Y:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius = shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius - Sphere_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        case Qt::Key_Z:
        {
            shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius = shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius - Sphere_R;
            emit updateIndex(selectedShapeIndex);
            break;
        }
        }
    }
}
void Urdf_editor::renderShape(const Shape &shape)
{
    // 初始化变换矩阵为单位矩阵
    QMatrix4x4 modelMatrix;

    // 如果有父节点，通过关节变换计算子节点的变换矩阵
    if (shape.joint.parent_id >= 0) {
        for (const auto &s : shapes) {
            if (s.id == shape.joint.parent_id) {
                // 创建关节的变换矩阵
                QMatrix4x4 jointMatrix;
                applyTransform(jointMatrix, shape.joint.parent_to_child_origin.xyz, shape.joint.parent_to_child_origin.rpy);

                // 计算子节点的变换矩阵
                modelMatrix = jointMatrix;
                break;
            }
        }
    }

    // 应用几何变换
    applyTransform(modelMatrix, shape.link.visuals.origin.xyz, shape.link.visuals.origin.rpy);
    modelMatrix.scale(QVector3D(1.0f, 1.0f, 1.0f)); // 如果需要缩放，可以调整这里的参数

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(modelMatrix.constData());

    // 绘制形状
    if (shape.type == Shape::Cube)
    {
        drawCube(shape);
    }
    else if (shape.type == Shape::Sphere)
    {
        drawSphere(shape);
    }
    else if (shape.type == Shape::Cylinder)
    {
        drawCylinder(shape);
    }

    glPopMatrix();
}
void Urdf_editor::keyPressEvent(QKeyEvent *event)
{
    emit KeyPress(event->key());
    if(selectedShapeIndex != -1)
    {
        if(MoveRotateMode==0)
            handleKey_Move(event->key());
        else
            handleKey_Rotate(event->key());
    }
    if(event->key()==Qt::Key_Plus)
        PressKey_Plus = true;
    else if(event->key()==Qt::Key_Minus)
        PressKey_Minus = true;
    if(PressKey_Plus)
    {
        handleKey_WHLR_Plus(event->key());
    }
    else if(PressKey_Minus)
    {
        handleKey_WHLR_Minus(event->key());
    }
    update();
    QWidget::keyPressEvent(event);
}

void Urdf_editor::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Plus)
        PressKey_Plus = false;
    if(event->key()==Qt::Key_Minus)
        PressKey_Minus = false;
    QWidget::keyReleaseEvent(event);
}
// 在鼠标点击事件中检测是否点击了某个形状
void Urdf_editor::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
    float minDistance = std::numeric_limits<float>::max();
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
    update();

}

void Urdf_editor::mouseMoveEvent(QMouseEvent *event)
{
    float dy = float(event->x() - lastMousePos.x()) / width();
    float dx = float(event->y() - lastMousePos.y()) / height();
    if (event->buttons() & Qt::LeftButton)
    {
        rotationAngleX -= dy * 180; // 绕 x 轴旋转
        rotationAngleY += dx * 180; // 绕 z 轴旋转
    }
    else if (event->buttons() & Qt::RightButton)
    {
        rotationAngleX -= dy * 180; // 绕 x 轴旋转
        rotationAngleY += dx * 180; // 绕 z 轴旋转
    }

    lastMousePos = event->pos();
    update();
}

void Urdf_editor::mouseReleaseEvent(QMouseEvent *event)
{
    // 处理鼠标释放事件
}

void Urdf_editor::wheelEvent(QWheelEvent *event)
{
    zoomFactor -= event->angleDelta().y() / 1200.0f;
    if (zoomFactor < 0.1f)
        zoomFactor = 0.1f;
    update();
}

void Urdf_editor::dragEnterEvent(QDragEnterEvent *event)
{

    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
}

void Urdf_editor::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText())
    {
        // 获取拖放数据
        QString text = event->mimeData()->text();
        shapes.push_back(currentShape);
        update();
        event->acceptProposedAction();
    }
}

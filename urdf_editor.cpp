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
    setAcceptDrops(true); // 启用拖放功能
    //    // 初始化立方体对象并设置属性
    //    cube.link.visuals.origin.xyz = QVector3D(0.0f, 0.0f, 0.0f);
    //    //cube.scale = QVector3D(1.0f, 1.0f, 1.0f);
    //    cube.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    //    cube.link.visuals.color = QColor(Qt::red);
    //    cube.link.visuals.geometry.box.size = QVector3D(10.0f,1.0f,1.0f);

    //    sphere.link.visuals.origin.xyz = QVector3D(1.0f,0.0f,0.0f);
    //    sphere.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    //    sphere.link.visuals.geometry.sphere.radius = 1;
    //    sphere.link.visuals.color = QColor(Qt::blue);

    //    cylinder.link.visuals.origin.xyz = QVector3D(1.0f,0.0f,0.0f);
    //    cylinder.link.visuals.origin.rpy = QVector3D(0.0f, 0.0f, 0.0f);
    //    cylinder.link.visuals.geometry.cylinder.radius = 1;
    //    cylinder.link.visuals.geometry.cylinder.length = 2;
    //    cylinder.link.visuals.color = QColor(Qt::green);

    //    //shapes.push_back(cube);
    //    //shapes.push_back(sphere);
    //    //shapes.push_back(cylinder);
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
    qDebug() << "2";
    currentShape = shape;
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

    QVector3D eye(0.0f, 0.0f, zoomFactor * 10.0f); // 相机位置
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
            //qDebug() << "lastselectedShapeIndex:" << lastselectedShapeIndex << "selectedShapeIndex:" << selectedShapeIndex;
            shapes[lastselectedShapeIndex].link.visuals.color = precolor;       // 恢复之前选中的形状的颜色
            precolor = shapes[selectedShapeIndex].link.visuals.color;           // 保存当前选中形状的原始颜色
            shapes[selectedShapeIndex].link.visuals.color = QColor(Qt::yellow); // 高亮显示新选中的形状
            lastselectedShapeIndex = selectedShapeIndex;
        }
        // glColor3f(shapes[i].link.visuals.color.redF(), shapes[i].link.visuals.color.greenF(), shapes[i].link.visuals.color.blueF()); // 设置OpenGL颜色
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

    // 绘制圆柱体的圆面
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, height);
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        glVertex3f(x, y, height);
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
        glVertex3f(x, y, 0.0f);
        glVertex3f(x, y, height);
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

void Urdf_editor::renderShape(const Shape &shape)
{
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

//    for (size_t i = 0; i < shapes.size(); ++i)
//    {
//        QVector3D shapePos = shapes[i].link.visuals.origin.xyz;
//        float distance = (shapePos - clickPos).length();
//        if (distance < minDistance)
//        {
//            //minDistance = distance;
//            closestShapeIndex = i;
//        }
//    }

//    if (closestShapeIndex != -1)
//    {
//        selectedShapeIndex = closestShapeIndex;
//        qDebug() << "Shape selected:" << closestShapeIndex << "at distance:" << minDistance;
//    }
//    else
//    {
//        selectedShapeIndex = -1;
//    }

    update();
}

//void Urdf_editor::mousePressEvent(QMouseEvent *event)
//{
//    // 获取鼠标点击位置
//    float mouseX = event->pos().x();
//    float mouseY = event->pos().y();

//    // 转换鼠标位置到 OpenGL 坐标系
//    GLint viewport[4];
//    glGetIntegerv(GL_VIEWPORT, viewport);
//    mouseY = viewport[3] - mouseY;

//    // 设置选择半径（可以根据需要调整）
//    float selectionRadius = 0.05f; // 假设选择半径为 0.05（在 -1 到 1 的范围内）

//    // 将鼠标位置转换为屏幕空间坐标
//    QVector2D mousePos((mouseX / viewport[2]) * 2 - 1, (mouseY / viewport[3]) * 2 - 1);

//    // 遍历所有顶点，检查是否在选中范围内
//    for (int i = 0; i < vertices.size() / 6; i++)
//    {
//        QVector4D screen = mvp.map(QVector4D(QVector3D(vertices[i * 6 + 0], vertices[i * 6 + 1], vertices[i * 6 + 2]), 1.0f));

//        // 将齐次坐标转换为笛卡尔坐标
//        if (screen.w() != 0.0f)
//        {
//            screen.setX(screen.x() / screen.w());
//            screen.setY(screen.y() / screen.w());
//            screen.setZ(screen.z() / screen.w());
//        }

//        // 检查顶点是否在中心矩形内
//        if (screen.x() > -0.5 && screen.x() < 0.5 && screen.y() > -0.5 && screen.y() < 0.5)
//        {
//            // 计算顶点在屏幕空间中的位置
//            QVector2D vertexPos(screen.x(), screen.y());

//            // 计算顶点与鼠标位置的距离
//            float distance = (vertexPos - mousePos).length();

//            // 如果顶点在选择半径内，则认为选中了形状
//            if (distance < selectionRadius)
//            {
//                selectedShapeIndex = i / (vertices.size() / 6 / shapes.size()); // 计算选中的形状索引
//                qDebug() << "Shape selected at index:" << selectedShapeIndex << "Distance:" << distance;
//                break; // 找到一个选中的形状后跳出循环
//            }
//        }
//    }

//    if (selectedShapeIndex == -1)
//    {
//        qDebug() << "No shape selected";
//    }

//    // 更新窗口
//    update();
//}

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

    qDebug() << "3";
    if (event->mimeData()->hasText())
    {
        // 获取拖放数据
        QString text = event->mimeData()->text();
        shapes.push_back(currentShape);
        update();
        event->acceptProposedAction();
    }
}

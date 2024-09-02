#include "urdf_editor.h"
#include "ui_urdf_editor.h"
#include <iostream>
#include <qdebug.h>
#include <cmath>
#include <GL/glu.h>
std::vector<Shape> shapes;

Urdf_editor::Urdf_editor(QWidget *parent) : QOpenGLWidget(parent), cube(Shape::Cube), sphere(Shape::Sphere), cylinder(Shape::Cylinder) ,
    frameCount(0), fps(0.0f)
{
    setMinimumSize(640, 480);
    zoomFactor = 1.0f;
    rotationAngleX = 0.0f;
    rotationAngleY = 0.0f;
    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true); // 启用拖放功能
    connect(&update_timer,SIGNAL(timeout()),this,SLOT(on_timeout()));
    update_timer.start(100);
    fps_timer.start();
}

void Urdf_editor::reset()
{
    shapes.clear();
    joints.clear();
    selectedShapeIndex = -1;
    lastselectedShapeIndex = -1;
}

void Urdf_editor::on_timeout()
{
    update();
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
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qDebug() << "OpenGL Error after initializing OpenGL functions: " << error;
    }

    // 编译和链接着色器
    bool success = m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shapes.vert");
    error = glGetError();
    if (!success || error != GL_NO_ERROR) {
        qDebug() << "Vertex Shader Error: " << m_shaderProgram.log();
        qDebug() << "OpenGL Error after adding vertex shader: " << error;
    }

    success = m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shapes.frag");
    error = glGetError();
    if (!success || error != GL_NO_ERROR) {
        qDebug() << "Fragment Shader Error: " << m_shaderProgram.log();
        qDebug() << "OpenGL Error after adding fragment shader: " << error;
    }

    success = m_shaderProgram.link();
    error = glGetError();
    if (!success || error != GL_NO_ERROR) {
        qDebug() << "Shader Program Link Error: " << m_shaderProgram.log();
        qDebug() << "OpenGL Error after linking shader program: " << error;
    }

    m_shaderProgram.bind();
    drawAxis();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // 设置背景颜色为灰色
}

void Urdf_editor::resizeGL(int w, int h)
{

}

void Urdf_editor::paintGL()
{
    // 清除颜色和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置投影和视图矩阵
    QMatrix4x4 projection;
    QMatrix4x4 view;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 1, 100);
    view.lookAt(m_camera.Positon, m_camera.Positon + m_camera.Front, m_camera.Up);

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("projection", projection);
    m_shaderProgram.setUniformValue("view", view);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // 先绘制坐标轴
    QMatrix4x4 model;
    m_shaderProgram.setUniformValue("model", model);
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

    // 绘制其他形状
    renderShapes();

    // 释放着色器程序
    m_shaderProgram.release();

    // 计算FPS
    frameCount++;
    if (fps_timer.elapsed() >= 1000) { // 每秒更新一次FPS
        fps = frameCount / (fps_timer.elapsed() / 1000.0f);
        frameCount = 0;
        fps_timer.restart();
    }

    // 保存OpenGL状态
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // 使用QPainter绘制FPS
    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16));
    painter.drawText(10, 20, QString("FPS: %1").arg(fps, 0, 'f', 2));

    // 结束QPainter的绘制
    painter.end();

    // 恢复OpenGL状态
    glPopAttrib();
}




void Urdf_editor::drawCube(const Shape &shape, QMatrix4x4 model) {
    // 设置顶点和颜色数据
    QVector3D size = shape.link.visuals.geometry.box.size;
    QVector3D color = QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());
    // 使用 MeshGenerator 生成方体网格
    static auto cubeMesh = MeshGenerator::generateCubeMesh(size,color);

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("model", model);

    QMatrix4x4 view;
    view.lookAt(m_camera.Positon, m_camera.Positon + m_camera.Front, m_camera.Up);
    m_shaderProgram.setUniformValue("view", view);

    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 0.1, 100);
    m_shaderProgram.setUniformValue("projection", projection);

    cubeMesh->Draw();

    m_shaderProgram.release();
}


inline double Urdf_editor::radiansToDegrees(double radians) {
    return radians * (180.0 / M_PI);
}

void Urdf_editor::drawSphere(const Shape &shape, QMatrix4x4 model) {
    auto sphereMesh = MeshGenerator::generateSphereMesh(
        shape.link.visuals.geometry.sphere.radius,
        30, 30,
        QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF())
        );

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("model", model);

    QMatrix4x4 view;
    view.lookAt(m_camera.Positon, m_camera.Positon + m_camera.Front, m_camera.Up);
    m_shaderProgram.setUniformValue("view", view);

    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 0.1, 100);
    m_shaderProgram.setUniformValue("projection", projection);

    sphereMesh->Draw();

    m_shaderProgram.release();
}

void Urdf_editor::drawCylinder(const Shape &shape, QMatrix4x4 model) {
    // 使用 MeshGenerator 生成圆柱体网格
    static auto cylinderMesh = MeshGenerator::generateCylinderMesh(
        shape.link.visuals.geometry.cylinder.radius,
        shape.link.visuals.geometry.cylinder.length,
        30,  // 分段数量，可以调整以增加细节
        QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF())
        );

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("model", model);

    QMatrix4x4 view;
    view.lookAt(m_camera.Positon, m_camera.Positon + m_camera.Front, m_camera.Up);
    m_shaderProgram.setUniformValue("view", view);

    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 0.1, 100);
    m_shaderProgram.setUniformValue("projection", projection);

    cylinderMesh->Draw();

    m_shaderProgram.release();
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
void Urdf_editor::drawGrid(float gridSize, float step)
{
    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("useUniformColor", true); // 使用统一颜色
    m_shaderProgram.setUniformValue("color", QVector3D(1.0f, 0.8f, 0.8f)); // 浅灰色

    glLineWidth(1.0f);
    glBegin(GL_LINES);

    // 绘制沿 X 轴方向的线条（在 XY 平面上）
    for (float i = -gridSize; i <= gridSize; i += step)
    {
        glVertex3f(i, -gridSize, 0.0f); // YZ 平面上的线
        glVertex3f(i, gridSize, 0.0f);
    }

    // 绘制沿 Y 轴方向的线条
    for (float i = -gridSize; i <= gridSize; i += step)
    {
        glVertex3f(-gridSize, i, 0.0f); // XZ 平面上的线
        glVertex3f(gridSize, i, 0.0f);
    }

    glEnd();

    m_shaderProgram.release();
}



void Urdf_editor::drawAxis()
{
    // === 初始化坐标轴 ===

    float axisVertices[] = {
        // X轴 - 红色
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 起点
        10.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 终点

        // Y轴 - 绿色
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // 起点
        0.0f, 10.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // 终点

        // Z轴 - 蓝色
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 起点
        0.0f, 0.0f, 10.0f,  0.0f, 0.0f, 1.0f   // 终点
    };

    // 生成坐标轴的VAO和VBO
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

    // 绑定着色器程序并传递变换矩阵
    m_shaderProgram.bind();
    QMatrix4x4 view;
    view.lookAt(m_camera.Positon, m_camera.Positon + m_camera.Front, m_camera.Up);

    //m_shaderProgram.setUniformValue("model", modelMatrix);
    m_shaderProgram.setUniformValue("view", view);

    // 绘制形状
    if (shape.type == Shape::Cube)
    {
        drawCube(shape, modelMatrix);
    }
    else if (shape.type == Shape::Sphere)
    {
        drawSphere(shape, modelMatrix);
    }
    else if (shape.type == Shape::Cylinder)
    {
        drawCylinder(shape, modelMatrix);
    }

    m_shaderProgram.release();
}

void Urdf_editor::keyPressEvent(QKeyEvent *event)
{
    float cameraSpeed = 100/1000.0;
    switch (event->key()) {
    case Qt::Key_W:m_camera.ProcessKeyborad(FORWARD,cameraSpeed);break;
    case Qt::Key_S:m_camera.ProcessKeyborad(BACKWARD,cameraSpeed);break;
    case Qt::Key_D:m_camera.ProcessKeyborad(RIGHT,cameraSpeed);break;
    case Qt::Key_A:m_camera.ProcessKeyborad(LEFT,cameraSpeed);break;

    default:
        break;
    }
    update();

}

void Urdf_editor::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Plus)
        PressKey_Plus = false;
    if(event->key()==Qt::Key_Minus)
        PressKey_Minus = false;
    QWidget::keyReleaseEvent(event);
}

//void Urdf_editor::drawAxis()
//{
//    glLineWidth(2.0);
//    glBegin(GL_LINES);

//    glColor3f(1.0, 0.0, 0.0); // X 轴
//    glVertex3f(0.0, 0.0, 0.0);
//    glVertex3f(100.0, 0.0, 0.0);
//    glEnd();
//    glBegin(GL_LINES);
//    glColor3f(0.0, 1.0, 0.0); // Y 轴
//    glVertex3f(0.0, 0.0, 0.0);
//    glVertex3f(0.0, 100.0, 0.0);
//    glEnd();

//    glColor3f(0.0, 0.0, 1.0); // Z 轴
//    glVertex3f(0.0, 0.0, 0.0);
//    glVertex3f(0.0, 0.0, 100.0);

//    glEnd();
//}

void Urdf_editor::renderShapes()
{
    for (size_t i = 0; i < shapes.size(); ++i) {
        renderShape(shapes[i]); // 渲染每个形状
    }
}

// 在鼠标点击事件中检测是否点击了某个形状
void Urdf_editor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 重置 lastPos 为鼠标按下时的位置
        lastPos = event->pos();
    }
}

void Urdf_editor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        //static QPoint lastPos(width() / 2, height() / 2);
        auto currentPos = event->pos();
        deltaPos = currentPos - lastPos;
        lastPos = currentPos;
        m_camera.ProcessMouseMoveMent(deltaPos.x(), -deltaPos.y(),GL_FALSE);
        update();
    }
}


void Urdf_editor::mouseReleaseEvent(QMouseEvent *event)
{
    // 处理鼠标释放事件
    if (event->button() == Qt::MiddleButton) {
        setCursor(Qt::ArrowCursor); // 恢复鼠标指针为箭头
    }
}

void Urdf_editor::wheelEvent(QWheelEvent *event)
{
    m_camera.ProcessMouseScroll(event->angleDelta().y()/120);
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

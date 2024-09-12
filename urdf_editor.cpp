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
void Urdf_editor::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);  // 使用默认的深度比较方式
    glDepthMask(GL_TRUE);   // 允许写入深度缓冲区
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qDebug() << "OpenGL Error after initializing OpenGL functions: " << error;
    }

    // 创建帧缓冲对象并绑定
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // 创建并配置第一个颜色附件 (普通颜色)
    glGenTextures(1, &frame_color_texture);
    glBindTexture(GL_TEXTURE_2D, frame_color_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_color_texture, 0);

    // 创建并配置第二个颜色附件 (ID颜色)
    glGenTextures(1, &frame_id_texture);
    glBindTexture(GL_TEXTURE_2D, frame_id_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width(), height(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, frame_id_texture, 0);

    // 创建深度和模板缓冲附件
    glGenTextures(1, &frame_depth_stencil_texture);
    glBindTexture(GL_TEXTURE_2D, frame_depth_stencil_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width(), height(), 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, frame_depth_stencil_texture, 0);

    // 设置绘制缓冲目标
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    // 检查帧缓冲完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qDebug() << "Frame buffer is not complete!";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // 解除绑定

    axis_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/axis.vert");
    axis_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/axis.frag");
    axis_shaderProgram.link();
    // 初始化坐标轴的 VAO 和 VBO
    drawAxis();
    // 编译和链接着色器
    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/shapes.vert");
    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/shapes.frag");
    m_shaderProgram.link();
    m_outlineShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/select.vert");
    m_outlineShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/select.frag");
    m_outlineShaderProgram.link();
    m_shaderProgram.bind();
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // 设置背景颜色为灰色
}


void Urdf_editor::resizeGL(int w, int h)
{

}

void Urdf_editor::paintGL()
{
    // 清除颜色和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_TRUE); // 启用深度缓冲区写入

    // 设置投影和视图矩阵
    QMatrix4x4 projection;
    QMatrix4x4 view;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 1, 100);
    view.lookAt(m_camera.Position, m_camera.Position + m_camera.Front, m_camera.Up);

    // === 使用 axis_shaderProgram 渲染坐标轴 ===
    axis_shaderProgram.bind();  // 绑定用于坐标轴的着色器

    // 设置坐标轴的 Uniforms
    QMatrix4x4 model;  // 坐标轴的模型矩阵，一般为单位矩阵
    axis_shaderProgram.setUniformValue("model", model);
    axis_shaderProgram.setUniformValue("view", view);
    axis_shaderProgram.setUniformValue("projection", projection);

    // 绘制坐标轴
    //glBindVertexArray(axisVAO);
    //glDrawArrays(GL_LINES, 0, 6);  // 绘制X、Y、Z轴（6个顶点）
    //glBindVertexArray(0);

    // 释放 axis_shaderProgram
    axis_shaderProgram.release();

    // === 使用 m_shaderProgram 渲染其他形状 ===
    m_shaderProgram.bind();  // 绑定用于几何形状的着色器
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // 设置几何体的 Uniforms
    m_shaderProgram.setUniformValue("view", view);
    m_shaderProgram.setUniformValue("projection", projection);

    // 绘制其他形状
    renderShapes();  // renderShapes() 内部应该负责设置模型矩阵和调用形状的渲染

    // 释放 m_shaderProgram
    m_shaderProgram.release();

    // === 计算并显示 FPS ===
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
QMatrix4x4 convertOpenGLToRViz() {
    QMatrix4x4 conversionMatrix;

    // OpenGL to RViz coordinate conversion
    conversionMatrix.setToIdentity();
    conversionMatrix.rotate(90.0f, 1.0f, 0.0f, 0.0f);  // X轴旋转90度，交换Y和Z轴

    return conversionMatrix;
}


void Urdf_editor::drawCube(const Shape &shape, QMatrix4x4 model) {
    int modelID = shape.id;

    // 设置顶点和颜色数据
    QVector3D size = shape.link.visuals.geometry.box.size;
    QQuaternion quaternion = shape.link.visuals.origin.quaternion;
    QVector3D color;
    if(shape.isSelected)
    {
        color = QVector3D(1, 1, 0);
        selectedShapeIndex = modelID;

        if(selectedShapeIndex != lastselectedShapeIndex)
        {
            lastselectedShapeIndex = selectedShapeIndex;
            emit updateIndex(selectedShapeIndex);
        }

        if(isMoveMode)
            drawConeAtCubeAxis(size, model);  // 新增的函数，用于在方块上绘制圆锥
        else if(isScaleMode)
            drawScaleAtCubeAxis(size, model);
        else if(isRotateMode)
            drawRotationRingsAtCubeAxis(size, quaternion, model);
        else
            drawAxisAtShape(model);
    }
    else
        color = QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());
    // 使用 MeshGenerator 生成方体网格
    auto cubeMesh = MeshGenerator::generateCubeMesh(size);

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("model", model);
    m_shaderProgram.setUniformValue("modelID", modelID);
    m_shaderProgram.setUniformValue("color", color);  // 传递颜色到着色器


    QMatrix4x4 view;
    view.lookAt(m_camera.Position, m_camera.Position + m_camera.Front, m_camera.Up);
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
    int modelID = shape.id;
    QVector3D color;
    float radius = shape.link.visuals.geometry.sphere.radius;
    QQuaternion quaternion = shape.link.visuals.origin.quaternion;
    if(shape.isSelected)
    {
        color = QVector3D(1, 1, 0);
        selectedShapeIndex = modelID;
        if(selectedShapeIndex != lastselectedShapeIndex)
        {
            lastselectedShapeIndex = selectedShapeIndex;
            emit updateIndex(selectedShapeIndex);
        }
        if(isMoveMode)
            drawConeAtSphereAxis(radius, model);
        else if (isScaleMode)
            drawScaleAtSphereAxis(radius, model);
        else if(isRotateMode)
            drawRotationRingsAtSphereAxis(radius, quaternion, model);
        else
            drawAxisAtShape(model);
    }
    else
        color = QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());
    auto sphereMesh = MeshGenerator::generateSphereMesh(
        radius,
        30, 30
        );

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("model", model);
    m_shaderProgram.setUniformValue("modelID", modelID);
    m_shaderProgram.setUniformValue("color", color);  // 传递颜色到着色器


    QMatrix4x4 view;
    view.lookAt(m_camera.Position, m_camera.Position + m_camera.Front, m_camera.Up);
    m_shaderProgram.setUniformValue("view", view);

    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 0.1, 100);
    m_shaderProgram.setUniformValue("projection", projection);

    sphereMesh->Draw();

    m_shaderProgram.release();
}

void Urdf_editor::drawCylinder(const Shape &shape, QMatrix4x4 model) {
    int modelID = shape.id;
    QVector3D color;
    float radius = shape.link.visuals.geometry.cylinder.radius;
    float length = shape.link.visuals.geometry.cylinder.length;
    QQuaternion quaternion = shape.link.visuals.origin.quaternion;
    if(shape.isSelected)
    {
        color = QVector3D(1, 1, 0);
        selectedShapeIndex = modelID;
        if(selectedShapeIndex != lastselectedShapeIndex)
        {
            lastselectedShapeIndex = selectedShapeIndex;
            emit updateIndex(selectedShapeIndex);
        }
        if(isMoveMode)
            drawConeAtCylinderAxis(radius, length, model);
        else if (isScaleMode)
            drawScaleAtCylinderAxis(radius, length, model);
        else if(isRotateMode)
            drawRotationRingsAtCylinderAxis(radius, length, quaternion, model);
        else
            drawAxisAtShape(model);
    }
    else
        color = QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());
    // 使用 MeshGenerator 生成圆柱体网格
    auto cylinderMesh = MeshGenerator::generateCylinderMesh(
        radius,
        length,
        30  // 分段数量，可以调整以增加细节
        );

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("model", model);
    m_shaderProgram.setUniformValue("modelID", modelID);
    m_shaderProgram.setUniformValue("color", color);  // 传递颜色到着色器


    QMatrix4x4 view;
    view.lookAt(m_camera.Position, m_camera.Position + m_camera.Front, m_camera.Up);
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

void Urdf_editor::drawAxisAtShape(const QMatrix4x4 &modelMatrix)
{
    // 使用 axis_shaderProgram 渲染坐标轴
    axis_shaderProgram.bind();

    // 设置投影矩阵和视图矩阵
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 0.1f, 100.0f);

    QMatrix4x4 view;
    view.lookAt(m_camera.Position, m_camera.Position + m_camera.Front, m_camera.Up);

    // 从物体的 modelMatrix 中仅提取平移部分，不应用旋转
    QMatrix4x4 axisModelMatrix;
    axisModelMatrix.translate(modelMatrix.column(3).toVector3D());  // 只使用平移部分

    axis_shaderProgram.setUniformValue("model", axisModelMatrix);
    axis_shaderProgram.setUniformValue("view", view);
    axis_shaderProgram.setUniformValue("projection", projection);

    // 绘制 X 轴并赋予唯一的颜色 ID
    axis_shaderProgram.setUniformValue("color", QVector3D(1.0f, 0.0f, 0.0f));  // 红色
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 2);  // 绘制X轴

    // 绘制 Y 轴并赋予唯一的颜色 ID
    axis_shaderProgram.setUniformValue("color", QVector3D(0.0f, 1.0f, 0.0f));  // 绿色
    glDrawArrays(GL_LINES, 2, 2);  // 绘制Y轴

    // 绘制 Z 轴并赋予唯一的颜色 ID
    axis_shaderProgram.setUniformValue("color", QVector3D(0.0f, 0.0f, 1.0f));  // 蓝色
    glDrawArrays(GL_LINES, 4, 2);  // 绘制Z轴

    glBindVertexArray(0);
    axis_shaderProgram.release();
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

void Urdf_editor::drawConeAtCubeAxis(const QVector3D& size, QMatrix4x4 model) {
    // 根据方块的尺寸调整圆锥的大小
    float coneRadius = 0.2f;  // 圆锥的半径为方块宽度的 20%
    float coneHeight = 0.8f;  // 圆锥的高度为方块高度的 80%
    float axisThickness = 0.05f;  // 长方体轴的厚度
    float axisLength = (std::max({size.x(), size.y(), size.z()}) + 2.0f)/2.0f;  // 最大值作为轴的统一长度

    // 生成圆锥网格
    auto coneMesh = MeshGenerator::generateConeMesh(coneRadius, coneHeight, 32);  // 动态生成圆锥
    // 生成长方体棍子的网格
    auto axisMesh = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, axisLength, axisThickness));  // 生成长方体的几何体

    m_shaderProgram.bind();
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 0.1f, 100.0f);

    // 设置颜色为红色（X轴圆锥），绿色（Y轴圆锥），蓝色（Z轴圆锥）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色
    model  = clearRotationAndKeepTranslation(model);

    // X轴上的长方体棍子和圆锥
    QMatrix4x4 xConeModel = model;
    xConeModel.translate(QVector3D(axisLength, 0, 0));  // 沿X轴正方向平移，平移距离为方块宽度的1.5倍
    xConeModel.rotate(-90, 0, 0, 1);  // 旋转使其对准X轴

    // 绘制长方体轴
    QMatrix4x4 xAxisModel = model;
    xAxisModel.translate(QVector3D(axisLength / 2.0f, 0, 0));  // 沿X轴平移，使其中心与圆锥对齐
    xAxisModel.rotate(-90, 0, 0, 1);  // 对准X轴
    m_shaderProgram.setUniformValue("model", xAxisModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    m_shaderProgram.setUniformValue("modelID", 10000);
    axisMesh->Draw();

    // 绘制X轴上的圆锥
    m_shaderProgram.setUniformValue("model", xConeModel);
    m_shaderProgram.setUniformValue("modelID", 10000);
    coneMesh->Draw();

    // Y轴上的长方体棍子和圆锥
    QMatrix4x4 yConeModel = model;
    yConeModel.translate(QVector3D(0, axisLength, 0));  // 沿Y轴正方向平移，平移距离为方块高度的1.5倍
    yConeModel.rotate(0, 1, 0, 0);  // 对齐 Y 轴

    // 绘制长方体轴
    QMatrix4x4 yAxisModel = model;
    yAxisModel.translate(QVector3D(0, axisLength / 2.0f , 0));  // 沿Y轴平移，使其中心与圆锥对齐
    m_shaderProgram.setUniformValue("model", yAxisModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    m_shaderProgram.setUniformValue("modelID", 10001);
    axisMesh->Draw();

    // 绘制Y轴上的圆锥
    m_shaderProgram.setUniformValue("model", yConeModel);
    m_shaderProgram.setUniformValue("modelID", 10001);
    coneMesh->Draw();

    // Z轴上的长方体棍子和圆锥
    QMatrix4x4 zConeModel = model;
    zConeModel.translate(QVector3D(0, 0, axisLength));  // 沿Z轴正方向平移，平移距离为方块深度的1.5倍
    zConeModel.rotate(90, QVector3D(1, 0, 0));  // 旋转使其对准Z轴

    // 绘制长方体轴
    QMatrix4x4 zAxisModel = model;
    zAxisModel.translate(QVector3D(0, 0, axisLength / 2.0f));  // 沿Z轴平移，使其中心与圆锥对齐
    zAxisModel.rotate(90, 1, 0, 0);  // 对准Z轴
    m_shaderProgram.setUniformValue("model", zAxisModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    m_shaderProgram.setUniformValue("modelID", 10002);
    axisMesh->Draw();

    // 绘制Z轴上的圆锥
    m_shaderProgram.setUniformValue("model", zConeModel);
    m_shaderProgram.setUniformValue("modelID", 10002);
    coneMesh->Draw();

    m_shaderProgram.release();
}

void Urdf_editor::drawConeAtSphereAxis(float radius, QMatrix4x4 model) {
    // 使用 MeshGenerator 生成圆锥网格
    float coneRadius = radius * 0.2f;  // 圆锥的半径为球体半径的 20%
    float coneHeight = radius * 0.8f;  // 圆锥的高度为球体半径的 80%
    float axisThickness = coneRadius * 0.5f;  // 长方体轴的厚度
    float axisLength = (radius + 2.0f) / 2.0f;  // 轴的统一长度

    // 生成圆锥网格
    auto coneMesh = MeshGenerator::generateConeMesh(coneRadius, coneHeight, 32);
    // 生成长方体网格
    auto axisMesh = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, axisLength, axisThickness));

    m_shaderProgram.bind();
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / height(), 0.1f, 100.0f);
    model  = clearRotationAndKeepTranslation(model);

    // 设置颜色为红色（X轴圆锥），绿色（Y轴圆锥），蓝色（Z轴圆锥）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色

    // X轴上的长方体和圆锥
    QMatrix4x4 xAxisModel = model;
    xAxisModel.translate(QVector3D(axisLength / 2.0f, 0, 0));  // 平移长方体使其中心对齐X轴
    xAxisModel.rotate(-90, 0, 0, 1);  // 旋转使其对准X轴
    m_shaderProgram.setUniformValue("model", xAxisModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMesh->Draw();

    QMatrix4x4 xConeModel = model;
    xConeModel.translate(QVector3D(axisLength, 0, 0));  // 沿X轴平移圆锥
    xConeModel.rotate(-90, 0, 0, 1);  // 旋转使其对准X轴
    m_shaderProgram.setUniformValue("model", xConeModel);
    coneMesh->Draw();

    // Y轴上的长方体和圆锥
    QMatrix4x4 yAxisModel = model;
    yAxisModel.translate(QVector3D(0, axisLength / 2.0f, 0));  // 平移长方体使其中心对齐Y轴
    m_shaderProgram.setUniformValue("model", yAxisModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMesh->Draw();

    QMatrix4x4 yConeModel = model;
    yConeModel.translate(QVector3D(0, axisLength, 0));  // 沿Y轴平移圆锥
    m_shaderProgram.setUniformValue("model", yConeModel);
    coneMesh->Draw();

    // Z轴上的长方体和圆锥
    QMatrix4x4 zAxisModel = model;
    zAxisModel.translate(QVector3D(0, 0, axisLength / 2.0f));  // 平移长方体使其中心对齐Z轴
    zAxisModel.rotate(90, 1, 0, 0);  // 旋转使其对准Z轴
    m_shaderProgram.setUniformValue("model", zAxisModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMesh->Draw();

    QMatrix4x4 zConeModel = model;
    zConeModel.translate(QVector3D(0, 0, axisLength));  // 沿Z轴平移圆锥
    zConeModel.rotate(90, QVector3D(1, 0, 0));  // 旋转使其对准Z轴
    m_shaderProgram.setUniformValue("model", zConeModel);
    coneMesh->Draw();

    m_shaderProgram.release();
}

void Urdf_editor::drawConeAtCylinderAxis(float radius, float height, QMatrix4x4 model) {
    // 使用 MeshGenerator 生成圆锥网格
    float coneRadius = radius * 0.2f;  // 圆锥的半径为圆柱体半径的 20%
    float coneHeight = height * 0.3f;  // 圆锥的高度为圆柱体高度的 30%
    float axisThickness = coneRadius * 0.5f;  // 长方体轴的厚度
    float axisLength = (height + 2.0f) / 2.0f;  // 轴的统一长度

    // 生成圆锥网格
    auto coneMesh = MeshGenerator::generateConeMesh(coneRadius, coneHeight, 32);
    // 生成长方体网格
    auto axisMesh = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, axisLength, axisThickness));
    model  = clearRotationAndKeepTranslation(model);

    m_shaderProgram.bind();
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / QWidget::height(), 0.1f, 100.0f);

    // 设置颜色为红色（X轴圆锥），绿色（Y轴圆锥），蓝色（Z轴圆锥）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色

    // X轴上的长方体和圆锥
    QMatrix4x4 xAxisModel = model;
    xAxisModel.translate(QVector3D(radius * 0.75, 0, 0));  // 沿X轴平移使长方体对齐
    xAxisModel.rotate(-90, 0, 0, 1);  // 旋转使其对准X轴
    m_shaderProgram.setUniformValue("model", xAxisModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMesh->Draw();

    QMatrix4x4 xConeModel = model;
    xConeModel.translate(QVector3D(radius * 1.5, 0, 0));  // 沿X轴正方向平移
    xConeModel.rotate(-90, 0, 0, 1);  // 旋转使其对准X轴
    m_shaderProgram.setUniformValue("model", xConeModel);
    coneMesh->Draw();

    // Y轴上的长方体和圆锥
    QMatrix4x4 yAxisModel = model;
    yAxisModel.translate(QVector3D(0, axisLength / 2.0f, 0));  // 平移长方体使其中心对齐Y轴
    m_shaderProgram.setUniformValue("model", yAxisModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMesh->Draw();

    QMatrix4x4 yConeModel = model;
    yConeModel.translate(QVector3D(0, axisLength, 0));  // 沿Y轴平移圆锥
    m_shaderProgram.setUniformValue("model", yConeModel);
    coneMesh->Draw();

    // Z轴上的长方体和圆锥
    QMatrix4x4 zAxisModel = model;
    zAxisModel.translate(QVector3D(0, 0, axisLength / 2.0f));  // 平移长方体使其中心对齐Z轴
    zAxisModel.rotate(90, 1, 0, 0);  // 旋转使其对准Z轴
    m_shaderProgram.setUniformValue("model", zAxisModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMesh->Draw();

    QMatrix4x4 zConeModel = model;
    zConeModel.translate(QVector3D(0, 0, axisLength));  // 沿Z轴平移圆锥
    zConeModel.rotate(90, QVector3D(1, 0, 0));  // 旋转使其对准Z轴
    m_shaderProgram.setUniformValue("model", zConeModel);
    coneMesh->Draw();

    m_shaderProgram.release();
}


void Urdf_editor::drawScaleAtCubeAxis(const QVector3D& size, QMatrix4x4 model) {
    // 找到 X、Y、Z 轴的最大长度值，并加上1单位的长度
    float maxLength = (std::max({size.x(), size.y(), size.z()}) + 2.0f)/2.0f;  // 最大值作为轴的统一长度
    float axisThickness = 1 * 0.05f;  // 长方体的厚度
    float cubeSize = 1 * 0.2f;  // 末端正方体的大小

    // 使用 generateCubeMesh 来生成不同大小的网格
    auto axisMeshX = MeshGenerator::generateCubeMesh(QVector3D(maxLength, axisThickness, axisThickness));  // X轴的长方体
    auto axisMeshY = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, maxLength, axisThickness));  // Y轴的长方体
    auto axisMeshZ = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, axisThickness, maxLength));  // Z轴的长方体

    // 生成正方体网格
    auto cubeMesh = MeshGenerator::generateCubeMesh(QVector3D(cubeSize, cubeSize, cubeSize));  // 动态生成正方体

    m_shaderProgram.bind();
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / QWidget::height(), 0.1f, 100.0f);

    // 设置颜色为红色（X轴），绿色（Y轴），蓝色（Z轴）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色
    // X轴上的长方体
    QMatrix4x4 xAxisModel = model;
    xAxisModel.translate(QVector3D(maxLength / 2.0f, 0, 0));  // 平移长方体到 X 轴的中心位置
    m_shaderProgram.setUniformValue("model", xAxisModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10000);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMeshX->Draw();

    // X轴上的末端正方体
    QMatrix4x4 xCubeModel = model;
    xCubeModel.translate(QVector3D(maxLength, 0, 0));  // 平移正方体到长方体末端
    m_shaderProgram.setUniformValue("model", xCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10000);
    cubeMesh->Draw();

    // Y轴上的长方体
    QMatrix4x4 yAxisModel = model;
    yAxisModel.translate(QVector3D(0, maxLength / 2.0f, 0));  // 平移长方体到 Y 轴的中心位置
    m_shaderProgram.setUniformValue("model", yAxisModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10001);
    axisMeshY->Draw();

    // Y轴上的末端正方体
    QMatrix4x4 yCubeModel = model;
    yCubeModel.translate(QVector3D(0, maxLength, 0));  // 平移正方体到长方体末端
    m_shaderProgram.setUniformValue("model", yCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10001);
    cubeMesh->Draw();

    // Z轴上的长方体
    QMatrix4x4 zAxisModel = model;
    zAxisModel.translate(QVector3D(0, 0, maxLength / 2.0f));  // 平移长方体到 Z 轴的中心位置
    zAxisModel.rotate(0, 1, 0, 0);  // 旋转使其对准 Z 轴
    m_shaderProgram.setUniformValue("model", zAxisModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10002);
    axisMeshZ->Draw();

    // Z轴上的末端正方体
    QMatrix4x4 zCubeModel = model;
    zCubeModel.translate(QVector3D(0, 0, maxLength));  // 平移正方体到长方体末端
    zCubeModel.rotate(90, 1, 0, 0);  // 旋转使其对准 Z 轴
    m_shaderProgram.setUniformValue("model", zCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10002);
    cubeMesh->Draw();

    m_shaderProgram.release();
}
void Urdf_editor::drawScaleAtSphereAxis(float radius, QMatrix4x4 model) {
    // 计算轴的最大长度，增加 1 个单位
    qDebug() << "radius : " << radius;
    float maxLength = (radius + 1.0f) ;  // 最大值作为轴的统一长度
    float axisThickness = 1 * 0.05f;  // 长方体的厚度
    float cubeSize = 1 * 0.2f;  // 末端正方体的大小

    // 使用 generateCubeMesh 来生成不同大小的网格
    auto axisMeshX = MeshGenerator::generateCubeMesh(QVector3D(maxLength, axisThickness, axisThickness));  // X轴的长方体
    auto axisMeshY = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, maxLength, axisThickness));  // Y轴的长方体
    auto axisMeshZ = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, axisThickness, maxLength));  // Z轴的长方体

    // 生成正方体网格
    auto cubeMesh = MeshGenerator::generateCubeMesh(QVector3D(cubeSize, cubeSize, cubeSize));  // 动态生成正方体

    m_shaderProgram.bind();
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / QWidget::height(), 0.1f, 100.0f);

    // 设置颜色为红色（X轴），绿色（Y轴），蓝色（Z轴）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色

    // X轴上的长方体
    QMatrix4x4 xAxisModel = model;
    xAxisModel.translate(QVector3D(maxLength / 2.0f, 0, 0));  // 平移长方体到 X 轴的中心位置
    m_shaderProgram.setUniformValue("model", xAxisModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10000);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMeshX->Draw();

    // X轴上的末端正方体
    QMatrix4x4 xCubeModel = model;
    xCubeModel.translate(QVector3D(maxLength, 0, 0));  // 平移正方体到长方体末端
    m_shaderProgram.setUniformValue("model", xCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10000);
    cubeMesh->Draw();

    // Y轴上的长方体
    QMatrix4x4 yAxisModel = model;
    yAxisModel.translate(QVector3D(0, maxLength / 2.0f, 0));  // 平移长方体到 Y 轴的中心位置
    m_shaderProgram.setUniformValue("model", yAxisModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10001);
    axisMeshY->Draw();

    // Y轴上的末端正方体
    QMatrix4x4 yCubeModel = model;
    yCubeModel.translate(QVector3D(0, maxLength, 0));  // 平移正方体到长方体末端
    m_shaderProgram.setUniformValue("model", yCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10001);
    cubeMesh->Draw();

    // Z轴上的长方体
    QMatrix4x4 zAxisModel = model;
    zAxisModel.translate(QVector3D(0, 0, maxLength / 2.0f));  // 平移长方体到 Z 轴的中心位置
    zAxisModel.rotate(0, 1, 0, 0);  // 旋转使其对准 Z 轴
    m_shaderProgram.setUniformValue("model", zAxisModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10002);
    axisMeshZ->Draw();

    // Z轴上的末端正方体
    QMatrix4x4 zCubeModel = model;
    zCubeModel.translate(QVector3D(0, 0, maxLength));  // 平移正方体到长方体末端
    zCubeModel.rotate(90, 1, 0, 0);  // 旋转使其对准 Z 轴
    m_shaderProgram.setUniformValue("model", zCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10002);
    cubeMesh->Draw();

    m_shaderProgram.release();
}
void Urdf_editor::drawScaleAtCylinderAxis(float radius, float height, QMatrix4x4 model) {
    // 计算轴的最大长度，增加 1 个单位
    float maxLength = (std::max(radius, height) + 1.0f) ;  // 最大值作为轴的统一长度
    float axisThickness = 1 * 0.05f;  // 长方体的厚度
    float cubeSize = 1 * 0.2f;  // 末端正方体的大小

    // 使用 generateCubeMesh 来生成不同大小的网格
    auto axisMeshX = MeshGenerator::generateCubeMesh(QVector3D(maxLength, axisThickness, axisThickness));  // X轴的长方体
    auto axisMeshY = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, maxLength, axisThickness));  // Y轴的长方体
    auto axisMeshZ = MeshGenerator::generateCubeMesh(QVector3D(axisThickness, axisThickness, maxLength));  // Z轴的长方体

    // 生成正方体网格
    auto cubeMesh = MeshGenerator::generateCubeMesh(QVector3D(cubeSize, cubeSize, cubeSize));  // 动态生成正方体

    m_shaderProgram.bind();
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / QWidget::height(), 0.1f, 100.0f);

    // 设置颜色为红色（X轴），绿色（Y轴），蓝色（Z轴）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色

    // X轴上的长方体
    QMatrix4x4 xAxisModel = model;
    xAxisModel.translate(QVector3D(maxLength / 2.0f, 0, 0));  // 平移长方体到 X 轴的中心位置
    m_shaderProgram.setUniformValue("model", xAxisModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10000);
    m_shaderProgram.setUniformValue("projection", projection);
    axisMeshX->Draw();

    // X轴上的末端正方体
    QMatrix4x4 xCubeModel = model;
    xCubeModel.translate(QVector3D(maxLength, 0, 0));  // 平移正方体到长方体末端
    m_shaderProgram.setUniformValue("model", xCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10000);
    cubeMesh->Draw();

    // Y轴上的长方体
    QMatrix4x4 yAxisModel = model;
    yAxisModel.translate(QVector3D(0, maxLength / 2.0f, 0));  // 平移长方体到 Y 轴的中心位置
    m_shaderProgram.setUniformValue("model", yAxisModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10001);
    axisMeshY->Draw();

    // Y轴上的末端正方体
    QMatrix4x4 yCubeModel = model;
    yCubeModel.translate(QVector3D(0, maxLength, 0));  // 平移正方体到长方体末端
    m_shaderProgram.setUniformValue("model", yCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10001);
    cubeMesh->Draw();

    // Z轴上的长方体
    QMatrix4x4 zAxisModel = model;
    zAxisModel.translate(QVector3D(0, 0, maxLength / 2.0f));  // 平移长方体到 Z 轴的中心位置
    zAxisModel.rotate(0, 1, 0, 0);  // 旋转使其对准 Z 轴
    m_shaderProgram.setUniformValue("model", zAxisModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10002);
    axisMeshZ->Draw();

    // Z轴上的末端正方体
    QMatrix4x4 zCubeModel = model;
    zCubeModel.translate(QVector3D(0, 0, maxLength));  // 平移正方体到长方体末端
    zCubeModel.rotate(90, 1, 0, 0);  // 旋转使其对准 Z 轴
    m_shaderProgram.setUniformValue("model", zCubeModel);
    m_shaderProgram.setUniformValue("modelID", 10002);
    cubeMesh->Draw();

    m_shaderProgram.release();
}
void Urdf_editor::drawRotationRingsAtCubeAxis(const QVector3D& size, const QQuaternion& rotation, QMatrix4x4 model) {
    // 找到 X、Y、Z 轴的最大长度值，并加上 1 单位的长度
    float maxLength = (std::max({size.x(), size.y(), size.z()}) + 1.0f) / 2.0f;  // 最大值作为轴的统一长度
    float ringThickness = 0.05f;  // 圆环的厚度
    float ringRadius = maxLength + 0.5f;  // 圆环的半径，稍大于轴

    // 使用 generateTorusMesh 来生成不同轴的旋转圆环
    auto ringMeshX = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // X轴的圆环
    auto ringMeshY = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // Y轴的圆环
    auto ringMeshZ = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // Z轴的圆环

    // 绑定着色器程序
    m_shaderProgram.bind();

    // 设置投影矩阵
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / QWidget::height(), 0.1f, 100.0f);

    // 将传入的四元数转换为旋转矩阵，并应用到模型矩阵上
    QMatrix4x4 rotationMatrix;
    rotationMatrix.rotate(rotation);  // 使用四元数生成旋转矩阵
    model  = clearRotationAndKeepTranslation(model);

    // 设置颜色为红色（X轴），绿色（Y轴），蓝色（Z轴）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色

    // 绘制X轴的旋转环（圆环）
    QMatrix4x4 xRingModel = model;
    xRingModel.rotate(90, 0, 1, 0);  // 旋转，使得圆环绕X轴对齐
    xRingModel.rotate(90, 0, 0, 1);  // 旋转，使得圆环绕X轴对齐

    m_shaderProgram.setUniformValue("model", xRingModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10000);  // X轴旋转环的ID
    m_shaderProgram.setUniformValue("projection", projection);
    ringMeshX->Draw();

    // 绘制Y轴的旋转环（圆环）
    QMatrix4x4 yRingModel = model;
    yRingModel.rotate(90, 1, 0, 0);  // 旋转，使得圆环绕Y轴对齐
    m_shaderProgram.setUniformValue("model", yRingModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10001);  // Y轴旋转环的ID
    ringMeshY->Draw();

    // 绘制Z轴的旋转环（圆环）
    QMatrix4x4 zRingModel = model;
    // Z轴方向的圆环默认对齐，无需旋转
    m_shaderProgram.setUniformValue("model", zRingModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10002);  // Z轴旋转环的ID
    ringMeshZ->Draw();

    // 释放着色器程序
    m_shaderProgram.release();
}

void Urdf_editor::drawRotationRingsAtSphereAxis(float radius, const QQuaternion& rotation, QMatrix4x4 model) {
    // 找到 X、Y、Z 轴的最大长度值，并加上 1 单位的长度
    float maxLength = (radius + 1.0f);  // 最大值作为轴的统一长度
    float ringThickness = 0.05f;  // 圆环的厚度
    float ringRadius = maxLength + 0.5f;  // 圆环的半径，稍大于轴

    // 使用 generateTorusMesh 来生成不同轴的旋转圆环
    auto ringMeshX = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // X轴的圆环
    auto ringMeshY = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // Y轴的圆环
    auto ringMeshZ = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // Z轴的圆环

    // 绑定着色器程序
    m_shaderProgram.bind();

    // 设置投影矩阵
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / QWidget::height(), 0.1f, 100.0f);

    // 将传入的四元数转换为旋转矩阵，并应用到模型矩阵上
    QMatrix4x4 rotationMatrix;
    rotationMatrix.rotate(rotation);  // 使用四元数生成旋转矩阵
    model  = clearRotationAndKeepTranslation(model);

    // 设置颜色为红色（X轴），绿色（Y轴），蓝色（Z轴）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色

    // 绘制X轴的旋转环（圆环）
    QMatrix4x4 xRingModel = model;
    xRingModel.rotate(90, 0, 1, 0);  // 旋转，使得圆环绕X轴对齐
    xRingModel.rotate(90, 0, 0, 1);  // 旋转，使得圆环绕X轴对齐

    m_shaderProgram.setUniformValue("model", xRingModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10000);  // X轴旋转环的ID
    m_shaderProgram.setUniformValue("projection", projection);
    ringMeshX->Draw();

    // 绘制Y轴的旋转环（圆环）
    QMatrix4x4 yRingModel = model;
    yRingModel.rotate(90, 1, 0, 0);  // 旋转，使得圆环绕Y轴对齐
    m_shaderProgram.setUniformValue("model", yRingModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10001);  // Y轴旋转环的ID
    ringMeshY->Draw();

    // 绘制Z轴的旋转环（圆环）
    QMatrix4x4 zRingModel = model;
    // Z轴方向的圆环默认对齐，无需旋转
    m_shaderProgram.setUniformValue("model", zRingModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10002);  // Z轴旋转环的ID
    ringMeshZ->Draw();

    // 释放着色器程序
    m_shaderProgram.release();
}

void Urdf_editor::drawRotationRingsAtCylinderAxis(float radius, float height, const QQuaternion& rotation, QMatrix4x4 model)
{
    // 找到 X、Y、Z 轴的最大长度值，并加上 1 单位的长度
    float maxLength = (std::max(radius, height) + 1.0f) ;  // 最大值作为轴的统一长度
    float ringThickness = 0.05f;  // 圆环的厚度
    float ringRadius = maxLength + 0.5f;  // 圆环的半径，稍大于轴

    // 使用 generateTorusMesh 来生成不同轴的旋转圆环
    auto ringMeshX = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // X轴的圆环
    auto ringMeshY = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // Y轴的圆环
    auto ringMeshZ = MeshGenerator::generateTorusMesh(ringRadius, ringThickness, 30, 30, 90);  // Z轴的圆环

    // 绑定着色器程序
    m_shaderProgram.bind();

    // 设置投影矩阵
    QMatrix4x4 projection;
    projection.perspective(m_camera.Zoom, (float)width() / QWidget::height(), 0.1f, 100.0f);

    // 将传入的四元数转换为旋转矩阵，并应用到模型矩阵上
    QMatrix4x4 rotationMatrix;
    rotationMatrix.rotate(rotation);  // 使用四元数生成旋转矩阵
    model  = clearRotationAndKeepTranslation(model);

    // 设置颜色为红色（X轴），绿色（Y轴），蓝色（Z轴）
    QVector3D xAxisColor(1, 0, 0);  // 红色
    QVector3D yAxisColor(0, 1, 0);  // 绿色
    QVector3D zAxisColor(0, 0, 1);  // 蓝色

    // 绘制X轴的旋转环（圆环）
    QMatrix4x4 xRingModel = model;
    xRingModel.rotate(90, 0, 1, 0);  // 旋转，使得圆环绕X轴对齐
    xRingModel.rotate(90, 0, 0, 1);  // 旋转，使得圆环绕X轴对齐

    m_shaderProgram.setUniformValue("model", xRingModel);
    m_shaderProgram.setUniformValue("color", xAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10000);  // X轴旋转环的ID
    m_shaderProgram.setUniformValue("projection", projection);
    ringMeshX->Draw();

    // 绘制Y轴的旋转环（圆环）
    QMatrix4x4 yRingModel = model;
    yRingModel.rotate(90, 1, 0, 0);  // 旋转，使得圆环绕Y轴对齐
    m_shaderProgram.setUniformValue("model", yRingModel);
    m_shaderProgram.setUniformValue("color", yAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10001);  // Y轴旋转环的ID
    ringMeshY->Draw();

    // 绘制Z轴的旋转环（圆环）
    QMatrix4x4 zRingModel = model;
    // Z轴方向的圆环默认对齐，无需旋转
    m_shaderProgram.setUniformValue("model", zRingModel);
    m_shaderProgram.setUniformValue("color", zAxisColor);
    m_shaderProgram.setUniformValue("modelID", 10002);  // Z轴旋转环的ID
    ringMeshZ->Draw();

    // 释放着色器程序
    m_shaderProgram.release();
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
void Urdf_editor::applyTransform(QMatrix4x4 &modelMatrix, const QVector3D &position, const QQuaternion &rotation) {
    // 应用平移
    modelMatrix.translate(position);

    // 应用四元数旋转
    modelMatrix.rotate(rotation);
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
    QQuaternion rotation = QQuaternion::fromEulerAngles(shape.link.visuals.origin.rpy);

    // 应用几何变换，使用位置和四元数
    applyTransform(modelMatrix, shape.link.visuals.origin.xyz, rotation);
    modelMatrix.scale(QVector3D(1.0f, 1.0f, 1.0f)); // 如果需要缩放，可以调整这里的参数
    // 绑定着色器程序并传递变换矩阵
    m_shaderProgram.bind();
    QMatrix4x4 view;
    view.lookAt(m_camera.Position, m_camera.Position + m_camera.Front, m_camera.Up);

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
    float cameraSpeed = 100 / 1000.0;
    switch (event->key()) {
    case Qt::Key_W:m_camera.ProcessKeyborad(FORWARD, cameraSpeed);break;
    case Qt::Key_S:m_camera.ProcessKeyborad(BACKWARD, cameraSpeed);break;
    case Qt::Key_D:m_camera.ProcessKeyborad(RIGHT, cameraSpeed);break;
    case Qt::Key_A:m_camera.ProcessKeyborad(LEFT, cameraSpeed);break;
    case Qt::Key_X:isXKeyPressed = true;isCameraCanMove = false;break;
    case Qt::Key_Y:isYKeyPressed = true;isCameraCanMove = false;break;
    case Qt::Key_Z:isZKeyPressed = true;isCameraCanMove = false;break;
    case Qt::Key_G:
        isCameraCanMove = false;
        // 切换到移动模式
        isFreeMoveMode = true;
        isMoveMode = true;
        isScaleMode = false;
        isRotateMode = false;
        break;
    case Qt::Key_R:
        // 切换到旋转模式
        isMoveMode = false;
        isScaleMode = false;
        isRotateMode = true;
        break;
    case Qt::Key_C:
        // 切换到缩放模式
        isMoveMode = false;
        isScaleMode = true;
        isRotateMode = false;
        break;
    default:
        break;
    }
    update();
}


void Urdf_editor::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_X:isXKeyPressed = false;isCameraCanMove = true;break;
    case Qt::Key_Y:isYKeyPressed = false;isCameraCanMove = true;break;
    case Qt::Key_Z:isZKeyPressed = false;isCameraCanMove = true;break;
    case Qt::Key_G:isFreeMoveMode = false;isCameraCanMove = true;break;
//    case Qt::Key_G:isMoveMode = false;break;
//    case Qt::Key_R:isRotateMode = false;break;
//    case Qt::Key_C:isScaleMode = false;break;
    default:
        QOpenGLWidget::keyReleaseEvent(event);
        break;
    }
    if(event->key()==Qt::Key_Plus)
        PressKey_Plus = false;
    if(event->key()==Qt::Key_Minus)
        PressKey_Minus = false;
    QWidget::keyReleaseEvent(event);
}

void Urdf_editor::renderShapes()
{
    QMatrix4x4 model;
    drawAxisAtShape(model); //
    for (size_t i = 0; i < shapes.size(); ++i) {
        renderShape(shapes[i]); // 渲染每个形状
    }
}

// 在鼠标点击事件中检测是否点击了某个形状
void Urdf_editor::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastPos = event->pos();

        // 绑定帧缓冲对象，确保渲染的是带有 ID 颜色的帧缓冲
        makeCurrent();
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // 清除颜色和深度缓冲区并渲染场景
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderShapes();  // 渲染所有形状到帧缓冲

        // 从颜色附件1中读取像素值（RGB分量）
        GLubyte pixel[3];
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glReadPixels(
            (int)event->pos().x(),
            this->height() - (int)event->pos().y(),
            1, 1,
            GL_RGB, GL_UNSIGNED_BYTE,
            pixel
            );

        // 重新组合RGB通道值为modelID
        int selectedModelID = (pixel[0] << 16) | (pixel[1] << 8) | pixel[2];
        qDebug() << "Selected model ID: " << selectedModelID;
        // 检查是否点击了某个轴上的圆锥
        if (selectedModelID == 10000) {
        isChooseX = true;
        isChooseY = false;
        isChooseZ = false;
        if(!isXKeyPressed&&!isYKeyPressed&&!isZKeyPressed)
        isCameraCanMove = false;
        } else if (selectedModelID == 10001) {
        isChooseX = false;
        isChooseY = true;
        isChooseZ = false;
        if(!isXKeyPressed&&!isYKeyPressed&&!isZKeyPressed)
            isCameraCanMove = false;
        } else if (selectedModelID == 10002) {
        isChooseX = false;
        isChooseY = false;
        isChooseZ = true;
        } else {
        isChooseX = false;
        isChooseY = false;
        isChooseZ = false;
        }
        if (selectedModelID < 10000)
            lastselectedModelID = selectedModelID;
        else
        {
            if(lastselectedModelID == -1);
            else
                selectedModelID = lastselectedModelID;
        }

        // 遍历模型，检测哪个模型的 ID 被选中
        int flag = 0;  // 标识是否有模型被选中
        for (auto &modelinfo : shapes) {
            if (modelinfo.id == selectedModelID) {
                if(modelinfo.type == Shape::Cube)
                    shapekind = 0;
                else if(modelinfo.type == Shape::Cylinder)
                    shapekind = 2;
                else if(modelinfo.type == Shape::Sphere)
                    shapekind = 1;
                flag = 1;
                modelinfo.isSelected = true;
                qDebug() << "The model is selected: " << " index: " << selectedModelID;
                if(!isChooseX && !isChooseY && !isChooseZ)
                    isCameraCanMove = true;
            } else {
                modelinfo.isSelected = false;
            }
        }
        if (flag == 0 ) {
            qDebug() << "No model is selected, index: " << selectedModelID;
        }

        // 操作执行完毕，切换回默认帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
        doneCurrent();
    }
}

QMatrix4x4 Urdf_editor::GetRotationMatrixFromRPY(float roll, float pitch, float yaw)
{
    QMatrix4x4 rotationMatrix;
    rotationMatrix.rotate(roll * 180.0f / M_PI, 1.0f, 0.0f, 0.0f);  // 绕X轴旋转
    rotationMatrix.rotate(pitch * 180.0f / M_PI, 0.0f, 1.0f, 0.0f); // 绕Y轴旋转
    rotationMatrix.rotate(yaw * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);   // 绕Z轴旋转
    return rotationMatrix;
}

void Urdf_editor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        auto currentPos = event->pos();
        deltaPos = currentPos - lastPos;  // 计算鼠标移动的增量
        lastPos = currentPos;  // 更新上一次鼠标位置

        // 检查是否处于自由移动模式
        if (isFreeMoveMode && selectedShapeIndex >= 0) {
            float moveSpeed = 0.01f;  // 控制移动速度
            // 获取摄像机的右向量和上向量
            QVector3D rightVector = m_camera.GetRightVector();  // 摄像机的右向量
            QVector3D upVector = m_camera.GetUpVector();        // 摄像机的上向量

            // 在自由移动模式下，鼠标左右移动对应沿着右向量移动，鼠标上下移动对应沿着上向量移动
            QVector3D deltaMove = rightVector * deltaPos.x() * moveSpeed + upVector * -deltaPos.y() * moveSpeed;

            // 更新模型的位置
            shapes[selectedShapeIndex].link.visuals.origin.xyz += deltaMove;
            qDebug() << "Free moving model, delta: " << deltaMove;
            emit updateIndex(selectedShapeIndex);
            update();  // 更新场景以反映模型的位置变化
        } else if (isMoveMode && selectedShapeIndex >= 0) {
            float moveSpeed = 0.01f;

            // 获取摄像机的右向量、上向量和前向量
            QVector3D rightVector = m_camera.GetRightVector();  // 摄像机的右向量
            QVector3D upVector = m_camera.GetUpVector();        // 摄像机的上向量

            QVector3D deltaMove = rightVector * deltaPos.x() * moveSpeed + upVector * -deltaPos.y() * moveSpeed;

            if (isXKeyPressed || isChooseX) {
                // 只沿摄像机的右向量移动对象，鼠标的 x 轴决定对象的移动
                shapes[selectedShapeIndex].link.visuals.origin.xyz.setX(
                    shapes[selectedShapeIndex].link.visuals.origin.xyz.x() + deltaMove.x()
                    );
            } else if (isYKeyPressed || isChooseY) {
                // 只沿摄像机的上向量移动对象，鼠标的 y 轴决定对象的移动
                shapes[selectedShapeIndex].link.visuals.origin.xyz.setY(
                    shapes[selectedShapeIndex].link.visuals.origin.xyz.y() + deltaMove.y()
                    );
            } else if (isZKeyPressed || isChooseZ) {
                // 只沿摄像机的前后向量移动对象，鼠标的 y 轴决定对象的移动
                shapes[selectedShapeIndex].link.visuals.origin.xyz.setZ(
                    shapes[selectedShapeIndex].link.visuals.origin.xyz.z() + deltaMove.z()
                    );
            }

            emit updateIndex(selectedShapeIndex);
        }
        else if (isScaleMode && selectedShapeIndex >= 0)
        {
            float scaleSpeed = 0.01f;

            // 获取摄像机的右向量、上向量
            QVector3D rightVector = m_camera.GetRightVector();  // 摄像机的右向量
            QVector3D upVector = m_camera.GetUpVector();        // 摄像机的上向量

//            // 计算鼠标移动引起的缩放变化
//            QVector3D deltaMove = rightVector * deltaPos.x() * scaleSpeed + upVector * -deltaPos.y() * scaleSpeed;

            QVector3D deltaMove = rightVector * deltaPos.x() * scaleSpeed + upVector * -deltaPos.y() * scaleSpeed;

            float roll = shapes[selectedShapeIndex].link.visuals.origin.rpy.x();
            float pitch = shapes[selectedShapeIndex].link.visuals.origin.rpy.y();
            float yaw = shapes[selectedShapeIndex].link.visuals.origin.rpy.z();

            QMatrix4x4 rotationMatrix = GetRotationMatrixFromRPY(roll, pitch, yaw);

            deltaMove = rotationMatrix * deltaMove;

            if (shapekind == 0)  // 处理盒子(Box)
            {
                if (isXKeyPressed || isChooseX) {
                    // 修改盒子的X轴尺寸
                    shapes[selectedShapeIndex].link.visuals.geometry.box.size.setX(
                        shapes[selectedShapeIndex].link.visuals.geometry.box.size.x() + deltaMove.x()
                        );
                } else if (isYKeyPressed || isChooseY) {
                    // 修改盒子的Y轴尺寸
                    shapes[selectedShapeIndex].link.visuals.geometry.box.size.setY(
                        shapes[selectedShapeIndex].link.visuals.geometry.box.size.y() + deltaMove.y()
                        );
                } else if (isZKeyPressed || isChooseZ) {
                    // 修改盒子的Z轴尺寸
                    shapes[selectedShapeIndex].link.visuals.geometry.box.size.setZ(
                        shapes[selectedShapeIndex].link.visuals.geometry.box.size.z() + deltaMove.z()
                        );
                }
            }
            else if (shapekind == 1)  // 处理球体(Sphere)
            {
                if (isXKeyPressed || isChooseX) {
                    // 修改盒子的X轴尺寸
                    shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius =
                        shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius + deltaMove.x();

                } else if (isYKeyPressed || isChooseY) {
                    // 修改盒子的Y轴尺寸
                    shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius =
                        shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius + deltaMove.y();
                } else if (isZKeyPressed || isChooseZ) {
                    // 修改盒子的Z轴尺寸
                    shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius =
                        shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius + deltaMove.z();
                }
                if(shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius<0)
                    shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius = -shapes[selectedShapeIndex].link.visuals.geometry.sphere.radius;
            }
            else if (shapekind == 2)  // 处理圆柱体(Cylinder)
            {
                {
                    if (isXKeyPressed || isChooseX) {
                        // 修改盒子的X轴尺寸
                        shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius =
                            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius + deltaMove.x();

                    } else if (isYKeyPressed || isChooseY) {
                        // 修改盒子的Y轴尺寸
                        shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length =
                            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length + deltaMove.y();
                    } else if (isZKeyPressed || isChooseZ) {
                        // 修改盒子的Z轴尺寸
                        shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius =
                            shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius + deltaMove.z();
                    }
                    if(shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius<0)
                        shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius = -shapes[selectedShapeIndex].link.visuals.geometry.cylinder.radius;
                    if(shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length<0)
                        shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length = -shapes[selectedShapeIndex].link.visuals.geometry.cylinder.length;
                }
            }
            // 发送更新信号以更新UI和场景
            emit updateIndex(selectedShapeIndex);
        }
        else if (isRotateMode && selectedShapeIndex >= 0) {
            float rotationSpeed = 0.005f;  // 控制旋转速度

            // 计算鼠标移动的增量
            // deltaPos = currentPos - lastPos;

            // 获取当前的旋转四元数
            QQuaternion currentRotation = shapes[selectedShapeIndex].link.visuals.origin.quaternion;

            // 根据选择的旋转轴来决定旋转行为
            QQuaternion deltaRotation;
            if (isChooseX || isXKeyPressed) {
                // 仅绕X轴旋转
                float angle = -deltaPos.y() * rotationSpeed;  // 使用Y轴的鼠标增量
                deltaRotation = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, qRadiansToDegrees(angle)); // 绕X轴旋转
            } else if (isChooseY || isYKeyPressed) {
                // 仅绕Y轴旋转
                float angle = -deltaPos.x() * rotationSpeed;  // 使用X轴的鼠标增量
                deltaRotation = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, qRadiansToDegrees(angle)); // 绕Y轴旋转
            } else if (isChooseZ || isZKeyPressed) {
                // 仅绕Z轴旋转
                float angle = deltaPos.x() * rotationSpeed;  // 使用X轴的鼠标增量
                deltaRotation = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, qRadiansToDegrees(angle)); // 绕Z轴旋转
            }

            // 将增量旋转应用到当前旋转
            currentRotation = deltaRotation * currentRotation;

            // 更新物体的四元数旋转状态
            shapes[selectedShapeIndex].link.visuals.origin.quaternion = currentRotation;

            // 更新UI时，才将四元数转换回欧拉角显示
            QVector3D newRPY = currentRotation.toEulerAngles();
            shapes[selectedShapeIndex].link.visuals.origin.rpy.setX((newRPY.x()));
            shapes[selectedShapeIndex].link.visuals.origin.rpy.setY(newRPY.y());
            shapes[selectedShapeIndex].link.visuals.origin.rpy.setZ(newRPY.z());

            // 发送更新信号以更新UI和场景
            emit updateIndex(selectedShapeIndex);
            update();
        }

        if((isXKeyPressed||isYKeyPressed||isZKeyPressed)||isFreeMoveMode||isChooseZ||isChooseX||isChooseY)
            isCameraCanMove = false;
        if (isCameraCanMove) {
            // 处理摄像机视角变化
            m_camera.ProcessMouseMoveMent(deltaPos.x(), -deltaPos.y(), GL_FALSE);
        }
        update();  // 更新场景
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
QMatrix4x4 Urdf_editor::clearRotationAndKeepTranslation(const QMatrix4x4& matrix) {
    QMatrix4x4 result;

    // 保持位移部分 (第四列前三个元素)
    result.setColumn(3, matrix.column(3));

    // 清除旋转和缩放，将前三列设置为单位矩阵
    result.setColumn(0, QVector4D(1, 0, 0, 0)); // x 轴方向
    result.setColumn(1, QVector4D(0, 1, 0, 0)); // y 轴方向
    result.setColumn(2, QVector4D(0, 0, 1, 0)); // z 轴方向

    return result;
}

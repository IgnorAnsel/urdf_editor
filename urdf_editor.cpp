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


    // 初始化投影矩阵
    projection.perspective(45.0, double(width()) / double(height()), 0.1, 100.0);

    // 初始化视图矩阵
    // 设置相机位置，使得Z轴朝上，X轴朝前，Y轴朝右
    center = QVector3D(0.0f, 0.0f, 0.0f);
    up = QVector3D(0.0f, 0.0f, 1.0f); // 将up向量设置为(0, 0, 1)，使Z轴朝上
    eye = QVector3D(0.0f, -zoomFactor * 10.0f, 0.0f); // 初始相机位置在Y轴方向负方向，指向Z轴方向

    viewMatrix.lookAt(eye, center, up);


    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("view", viewMatrix);
    m_shaderProgram.setUniformValue("projection", projection);
    m_shaderProgram.release();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // 禁用面剔除，确保所有面都被渲染
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // 设置背景颜色为灰色
}

void Urdf_editor::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    projection.setToIdentity();
    projection.perspective(45.0, double(w) / double(h), 0.1, 100.0);

    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("projection", projection);
    m_shaderProgram.release();
}

void Urdf_editor::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 更新视图矩阵
    QMatrix4x4 transform;
    transform.translate(translation);
    transform.rotate(rotationAngleX, 1.0f, 0.0f, 0.0f); // 绕X轴旋转
    transform.rotate(rotationAngleZ, 0.0f, 0.0f, 1.0f); // 绕Z轴旋转

    eye = QVector3D(0.0f, -zoomFactor * 10.0f, 0.0f); // 相机位置保持不变

    viewMatrix.setToIdentity();
    viewMatrix.lookAt(eye, center, up);
    viewMatrix *= transform;


    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("view", viewMatrix);

    // 绘制背景网格
    glDisable(GL_DEPTH_TEST); // 禁用深度测试，确保网格总是在最底层
    drawGrid(10.0f, 1.0f);   // 调用绘制网格的函数，网格大小为100，步长为1
    //drawPlane(10,10,1);
    glEnable(GL_DEPTH_TEST);  // 重新启用深度测试

    // 渲染坐标轴
    drawAxis();

    // 渲染形状
    renderShapes();
    m_shaderProgram.release();

}



//void Urdf_editor::drawCube(const Shape &shape)
//{
//    // 设置颜色
//    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

//    QVector3D size = shape.link.visuals.geometry.box.size;

//    glBegin(GL_QUADS);

//    // Front face
//    glVertex3f(-size.x() / 2, -size.y() / 2, size.z() / 2);
//    glVertex3f(size.x() / 2, -size.y() / 2, size.z() / 2);
//    glVertex3f(size.x() / 2, size.y() / 2, size.z() / 2);
//    glVertex3f(-size.x() / 2, size.y() / 2, size.z() / 2);

//    // Back face
//    glVertex3f(-size.x() / 2, -size.y() / 2, -size.z() / 2);
//    glVertex3f(-size.x() / 2, size.y() / 2, -size.z() / 2);
//    glVertex3f(size.x() / 2, size.y() / 2, -size.z() / 2);
//    glVertex3f(size.x() / 2, -size.y() / 2, -size.z() / 2);

//    // Top face
//    glVertex3f(-size.x() / 2, size.y() / 2, -size.z() / 2);
//    glVertex3f(-size.x() / 2, size.y() / 2, size.z() / 2);
//    glVertex3f(size.x() / 2, size.y() / 2, size.z() / 2);
//    glVertex3f(size.x() / 2, size.y() / 2, -size.z() / 2);

//    // Bottom face
//    glVertex3f(-size.x() / 2, -size.y() / 2, -size.z() / 2);
//    glVertex3f(size.x() / 2, -size.y() / 2, -size.z() / 2);
//    glVertex3f(size.x() / 2, -size.y() / 2, size.z() / 2);
//    glVertex3f(-size.x() / 2, -size.y() / 2, size.z() / 2);

//    // Right face
//    glVertex3f(size.x() / 2, -size.y() / 2, -size.z() / 2);
//    glVertex3f(size.x() / 2, size.y() / 2, -size.z() / 2);
//    glVertex3f(size.x() / 2, size.y() / 2, size.z() / 2);
//    glVertex3f(size.x() / 2, -size.y() / 2, size.z() / 2);

//    // Left face
//    glVertex3f(-size.x() / 2, -size.y() / 2, -size.z() / 2);
//    glVertex3f(-size.x() / 2, -size.y() / 2, size.z() / 2);
//    glVertex3f(-size.x() / 2, size.y() / 2, size.z() / 2);
//    glVertex3f(-size.x() / 2, size.y() / 2, -size.z() / 2);

//    glEnd();
//}
//void Urdf_editor::drawSphere(const Shape &shape)
//{
//    // 设置颜色
//    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

//    double radius = shape.link.visuals.geometry.sphere.radius;
//    const int slices = 30;
//    const int stacks = 30;

//    for (int i = 0; i <= stacks; ++i)
//    {
//        double lat0 = M_PI * (-0.5 + (double)(i - 1) / stacks);
//        double z0 = sin(lat0);
//        double zr0 = cos(lat0);

//        double lat1 = M_PI * (-0.5 + (double)i / stacks);
//        double z1 = sin(lat1);
//        double zr1 = cos(lat1);

//        glBegin(GL_QUAD_STRIP);
//        for (int j = 0; j <= slices; ++j)
//        {
//            double lng = 2 * M_PI * (double)(j - 1) / slices;
//            double x = cos(lng);
//            double y = sin(lng);

//            glNormal3d(x * zr0, y * zr0, z0);
//            glVertex3d(x * zr0 * radius, y * zr0 * radius, z0 * radius);

//            glNormal3d(x * zr1, y * zr1, z1);
//            glVertex3d(x * zr1 * radius, y * zr1 * radius, z1 * radius);
//        }
//        glEnd();
//    }
//}
//void Urdf_editor::drawCylinder(const Shape &shape)
//{
//    // 设置颜色
//    glColor3f(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

//    double radius = shape.link.visuals.geometry.cylinder.radius;
//    double height = shape.link.visuals.geometry.cylinder.length;
//    const int slices = 30;
//    double halfHeight = height / 2.0;

//    // 绘制圆柱体的底面
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0.0f, 0.0f, -halfHeight);
//    for (int i = 0; i <= slices; ++i)
//    {
//        double angle = 2.0 * M_PI * i / slices;
//        double x = radius * cos(angle);
//        double y = radius * sin(angle);
//        glVertex3f(x, y, -halfHeight);
//    }
//    glEnd();

//    // 绘制圆柱体的顶面
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0.0f, 0.0f, halfHeight);
//    for (int i = 0; i <= slices; ++i)
//    {
//        double angle = 2.0 * M_PI * i / slices;
//        double x = radius * cos(angle);
//        double y = radius * sin(angle);
//        glVertex3f(x, y, halfHeight);
//    }
//    glEnd();

//    // 绘制圆柱体的侧面
//    glBegin(GL_QUAD_STRIP);
//    for (int i = 0; i <= slices; ++i)
//    {
//        double angle = 2.0 * M_PI * i / slices;
//        double x = radius * cos(angle);
//        double y = radius * sin(angle);
//        glNormal3f(x, y, 0.0f);
//        glVertex3f(x, y, -halfHeight);
//        glVertex3f(x, y, halfHeight);
//    }
//    glEnd();
//}
void Urdf_editor::drawCube(const Shape &shape)
{
    // 设置顶点和颜色数据
    QVector3D size = shape.link.visuals.geometry.box.size;
    QVector3D color = QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());

    GLfloat vertices[] = {
        // Positions          // Colors
        -size.x() / 2, -size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),
        size.x() / 2, -size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),
        size.x() / 2,  size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),
        -size.x() / 2,  size.y() / 2,  size.z() / 2,  color.x(), color.y(), color.z(),
        -size.x() / 2, -size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z(),
        size.x() / 2, -size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z(),
        size.x() / 2,  size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z(),
        -size.x() / 2,  size.y() / 2, -size.z() / 2,  color.x(), color.y(), color.z(),
    };

    GLuint indices[] = {
        0, 1, 2, 2, 3, 0, // Front face
        4, 5, 6, 6, 7, 4, // Back face
        0, 4, 7, 7, 3, 0, // Left face
        1, 5, 6, 6, 2, 1, // Right face
        0, 1, 5, 5, 4, 0, // Bottom face
        3, 2, 6, 6, 7, 3  // Top face
    };
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 绑定着色器程序并传递变换矩阵
    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("useUniformColor", false); // 使用顶点颜色

    QMatrix4x4 model;
    model.setToIdentity();
    m_shaderProgram.setUniformValue("model", model);
    m_shaderProgram.setUniformValue("view", viewMatrix);
    m_shaderProgram.setUniformValue("projection", projection);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_shaderProgram.release();

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

inline double Urdf_editor::radiansToDegrees(double radians) {
    return radians * (180.0 / M_PI);
}

void Urdf_editor::drawSphere(const Shape &shape)
{
    // 提取颜色和半径
    QVector3D color = QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());
    double radius = shape.link.visuals.geometry.sphere.radius;
    const int slices = 30;  // 经度方向的切片数
    const int stacks = 30;  // 纬度方向的切片数

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    // 计算球体顶点和索引
    for (int i = 0; i <= stacks; ++i)
    {
        double lat = M_PI * (-0.5 + (double)i / stacks);  // 纬度从 -π/2 到 π/2
        double z = sin(lat) * radius;
        double zr = cos(lat) * radius;

        for (int j = 0; j <= slices; ++j)
        {
            double lng = 2 * M_PI * (double)j / slices;  // 经度从 0 到 2π
            double x = cos(lng) * zr;
            double y = sin(lng) * zr;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(color.x());
            vertices.push_back(color.y());
            vertices.push_back(color.z());
        }
    }

    // 生成索引数据
    for (int i = 0; i < stacks; ++i)
    {
        for (int j = 0; j < slices; ++j)
        {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // 生成和绑定VAO、VBO和EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 绑定顶点数据到VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

    // 绑定索引数据到EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 绑定着色器程序并传递变换矩阵
    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("useUniformColor", false);

    QMatrix4x4 model;
    model.setToIdentity();
    m_shaderProgram.setUniformValue("model", model);
    m_shaderProgram.setUniformValue("view", viewMatrix);
    m_shaderProgram.setUniformValue("projection", projection);

    // 绘制球体
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_shaderProgram.release();

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Urdf_editor::drawCylinder(const Shape &shape)
{
    // 提取颜色、半径和高度
    QVector3D color = QVector3D(shape.link.visuals.color.redF(), shape.link.visuals.color.greenF(), shape.link.visuals.color.blueF());
    double radius = shape.link.visuals.geometry.cylinder.radius;
    double height = shape.link.visuals.geometry.cylinder.length;
    const int slices = 30;
    double halfHeight = height / 2.0;

    std::vector<GLfloat> vertices;

    // 计算底面顶点
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(-halfHeight);
    vertices.push_back(color.x());
    vertices.push_back(color.y());
    vertices.push_back(color.z());

    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(-halfHeight);
        vertices.push_back(color.x());
        vertices.push_back(color.y());
        vertices.push_back(color.z());
    }

    // 计算顶面顶点
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(halfHeight);
    vertices.push_back(color.x());
    vertices.push_back(color.y());
    vertices.push_back(color.z());

    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(halfHeight);
        vertices.push_back(color.x());
        vertices.push_back(color.y());
        vertices.push_back(color.z());
    }

    // 计算侧面顶点
    for (int i = 0; i <= slices; ++i)
    {
        double angle = 2.0 * M_PI * i / slices;
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(-halfHeight);
        vertices.push_back(color.x());
        vertices.push_back(color.y());
        vertices.push_back(color.z());

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(halfHeight);
        vertices.push_back(color.x());
        vertices.push_back(color.y());
        vertices.push_back(color.z());
    }

    // 生成和绑定VAO、VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定VAO
    glBindVertexArray(VAO);

    // 绑定顶点数据到VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);

    // 绑定VBO到VAO并设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 *sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // 解绑VAO（在绘制之前，可以解除绑定）
    glBindVertexArray(0);

    // 绑定着色器程序并传递变换矩阵
    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("useUniformColor", false); // 使用顶点颜色

    QMatrix4x4 model;
    model.setToIdentity();  // 设置模型矩阵为单位矩阵
    m_shaderProgram.setUniformValue("model", model);
    m_shaderProgram.setUniformValue("view", viewMatrix);  // 设置视图矩阵
    m_shaderProgram.setUniformValue("projection", projection);  // 设置投影矩阵

    // 重新绑定VAO并绘制
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, slices + 2); // 绘制底面
    glDrawArrays(GL_TRIANGLE_FAN, slices + 2, slices + 2); // 绘制顶面
    glDrawArrays(GL_QUAD_STRIP, 2 * (slices + 2), 2 * (slices + 1)); // 绘制侧面

    glBindVertexArray(0); // 解除VAO绑定

    m_shaderProgram.release();  // 释放着色器程序

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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
    m_shaderProgram.bind();
    m_shaderProgram.setUniformValue("useUniformColor", true); // 使用统一颜色

    glLineWidth(2.0f);

    // X 轴 - 红色 (朝前)
    m_shaderProgram.setUniformValue("color", QVector3D(1.0f, 0.0f, 0.0f));
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);   // 原点
    glVertex3f(100.0f, 0.0f, 0.0f); // 朝前的X轴
    glEnd();

    // Y 轴 - 绿色 (朝右)
    m_shaderProgram.setUniformValue("color", QVector3D(0.0f, 1.0f, 0.0f));
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);   // 原点
    glVertex3f(0.0f, 100.0f, 0.0f); // 朝右的Y轴
    glEnd();

    // Z 轴 - 蓝色 (朝上)
    m_shaderProgram.setUniformValue("color", QVector3D(0.0f, 0.0f, 1.0f));
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);   // 原点
    glVertex3f(0.0f, 0.0f, 100.0f); // 朝上的Z轴
    glEnd();

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
    lastMousePos = event->pos();
    float minDistance = std::numeric_limits<float>::max();
    int closestShapeIndex = -1;
    if (event->button() == Qt::MiddleButton) {
        setCursor(Qt::ClosedHandCursor); // 改变鼠标指针为手型
    }
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
    float dx = float(event->x() - lastMousePos.x()) / width();
    float dy = float(event->y() - lastMousePos.y()) / height();

    if (event->buttons() & Qt::LeftButton)
    {
        rotationAngleX += dy * 180; // 调整符号，使上下旋转方向符合直觉
        rotationAngleZ += dx * 180; // 调整符号，使左右旋转方向符合直觉
    }
    else if (event->buttons() & Qt::RightButton)
    {
        rotationAngleX += dy * 180; // 调整符号，使上下旋转方向符合直觉
        rotationAngleZ += dx * 180; // 调整符号，使左右旋转方向符合直觉
    }
    else if (event->buttons() & Qt::MiddleButton) {
        translation.setX(translation.x() + dx * 10.0f); // 更新平移值 (左右平移)
        translation.setY(translation.y() - dy * 10.0f); // 更新平移值 (上下平移)
    }

    lastMousePos = event->pos();
    update();
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

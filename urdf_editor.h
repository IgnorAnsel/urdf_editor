#ifndef URDF_EDITOR_H
#define URDF_EDITOR_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include "shape.h"
#include "param_struct.h"
#include <QMimeData>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include <QTimer>
#include "meshgenerator.h"
#include <QElapsedTimer>
#include <QPainter>
extern std::vector<Shape> shapes;
extern std::vector<URDFJoint> joints;
class Urdf_editor : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
    Q_OBJECT
public:
    explicit Urdf_editor(QWidget *parent = nullptr);
    void reset();
public slots:
    void on_timeout();
    void receiveShapeKind(int kind);
    void updateWHLRStep(float cube_W ,
                        float cube_H ,
                        float cube_L ,
                        float cyliner_H ,
                        float cyliner_R ,
                        float sphere_R );
    void changeStep(int value);
    void ChangeMoveRotate(bool mode);
    void updateShape(); // 定义槽
    void receiveIndex(int index);
    void dropCreate(const Shape &shape);
    void updateJoint();
    void set_set_selectedShapeIndex_f1();
signals:
    void KeyPress(int keyvalue);
    void updateIndex(int index);
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void drawCube(const Shape &shape, QMatrix4x4 model);
    void drawSphere(const Shape &shape, QMatrix4x4 model);
    void drawCylinder(const Shape &shape, QMatrix4x4 model);
    void drawArrow(float x, float y, float z, float dx, float dy, float dz, QColor color);
    void drawCone(float height, float radius, QColor color);
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
private:

    QTimer update_timer; // 定时器（用于更新画面）
    unsigned int axisVAO, axisVBO; // 轴的顶点数组对象和顶点缓冲对象
    QPoint deltaPos;
    QPoint lastPos;

    Camera m_camera;
    QVector3D translation = QVector3D(0,0,0); // 平移变量
    QOpenGLShaderProgram m_shaderProgram;
    void drawAxis();
    void renderShapes();
    void drawGrid(float gridSize, float step);
    bool MoveRotateMode = 0;
    int selectedShapeIndex = -1; // -1 表示没有选中任何形状
    int lastselectedShapeIndex = -1;
    inline double radiansToDegrees(double radians);
    void renderShape(const Shape &shape);
    void drawPlane(float width, float height, float gridSize);
    void applyTransform(QMatrix4x4 &matrix, const QVector3D &translation, const QVector3D &rotation);
    void handleKey_Move(int key);
    void handleKey_Rotate(int key);
    void handleKey_WHLR_Plus(int key);
    void handleKey_WHLR_Minus(int key);
    QPoint lastMousePos;
    float zoomFactor;
    float rotationAngleX;
    float rotationAngleY;
    float rotationAngleZ;
    int shapekind;
    Shape cube; // 将cube定义为类的成员变量
    Shape sphere;
    Shape cylinder;
    Shape currentShape;
    QMatrix4x4 projection;
    QColor precolor;
    float Movestep = 0.5;
    float Rotatestep = 0.1;
    float Cube_W = 0.1;
    float Cube_H = 0.1;
    float Cube_L = 0.1;
    float Cyliner_H = 0.1;
    float Cyliner_R = 0.1;
    float Sphere_R = 0.1;

    //
    bool PressKey_Plus = false;
    bool PressKey_Minus = false;

    float coneHeight = 0.5f;
    float coneRadius = 0.1f;

    // 显示帧率
    int frameCount;
    float fps;
    QElapsedTimer fps_timer;
};

#endif // URDF_EDITOR_H

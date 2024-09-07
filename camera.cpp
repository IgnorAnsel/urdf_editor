#include "camera.h"

Camera::Camera(QVector3D postion, QVector3D up, float yaw, float pitch)
{
    Position = postion;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    MouseSensitivity = SENSITIVITY;
    MovementSpeed = SPEED;
    Zoom = ZOOM;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
    Position = QVector3D(posX,posY,posZ);
    WorldUp = QVector3D(upX,upY,upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

QVector3D Camera::GetForwardVector()
{
    return Front;
}

QVector3D Camera::GetUpVector()
{
    return Up;
}

QVector3D Camera::GetRightVector()
{
    return Right;
}

QMatrix4x4 Camera::GetViewMatrix()
{
    QMatrix4x4 theMatrix;
    theMatrix.lookAt(Position, Position + Front, Up);
    return theMatrix;
}

QMatrix4x4 Camera::GetProjectionMatrix(float Ratio)
{
    QMatrix4x4 projection;
    projection.perspective(Zoom, Ratio, 0.1, 100);
}

void Camera::ProcessKeyborad(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if(direction == FORWARD)
        Position += Front * velocity;
    if(direction == BACKWARD)
        Position -= Front * velocity;
    if(direction == LEFT)
        Position -= Right * velocity;
    if(direction == RIGHT)
        Position += Right * velocity;
    qDebug() << Position << "Pitch:"<< Pitch <<"Yaw:"<< Yaw << "Zoom" << Zoom;
}

void Camera::ProcessMouseScroll(float yoffset)
{
    if(Zoom >=1.0f&&Zoom<=75.0f)
        Zoom -= yoffset;
    if(Zoom <=1.0f)
        Zoom = 1.0f;
    if(Zoom >= 75.0f)
        Zoom = 75.0f;
}

void Camera::ProcessMouseMoveMent(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw -= xoffset;
    Pitch += yoffset;

    if(constrainPitch)
    {
        if(Pitch > 89.0f)
            Pitch = 89.0f;
        if(Pitch < -89.0f)
            Pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    float PI = 3.1415926;

    // 重新计算前向量，在 RViz 中 Z 轴是上，X 轴是前，Y 轴是左
    QVector3D front;
    front.setX(cos(Yaw * PI / 180.0f) * cos(Pitch * PI / 180.0f));  // X轴前后
    front.setY(sin(Yaw * PI / 180.0f) * cos(Pitch * PI / 180.0f));  // Y轴左右
    front.setZ(sin(Pitch * PI / 180.0f));                           // Z轴上下
    Front = front.normalized();

    // 右向量，使用世界坐标的 Z 轴 (0, 0, 1)
    Right = QVector3D::crossProduct(Front, QVector3D(0.0f, 0.0f, 1.0f)).normalized();

    // 上向量，通过 Right 和 Front 计算
    Up = QVector3D::crossProduct(Right, Front).normalized();
}


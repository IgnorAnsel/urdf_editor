#include "camera.h"

Camera::Camera(QVector3D postion, QVector3D up, float yaw, float pitch)
{
    Positon = postion;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    MouseSensitivity = SENSITIVITY;
    MovementSpeed = SPEED;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
    Positon = QVector3D(posX,posY,posZ);
    WorldUp = QVector3D(upX,upY,upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

QMatrix4x4 Camera::GetViewMatrix()
{
    QMatrix4x4 theMatrix;
    theMatrix.lookAt(Positon, Positon + Front, Up);
    return theMatrix;
}

void Camera::ProcessKeyborad(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if(direction == FORWARD)
        Positon += Front * velocity;
    if(direction == BACKWARD)
        Positon -= Front * velocity;
    if(direction == LEFT)
        Positon -= Right * velocity;
    if(direction == RIGHT)
        Positon += Right * velocity;
    qDebug() << direction;
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

    Yaw += xoffset;
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
    QVector3D front;
    front.setX(cos(Yaw*PI/180)* cos(Pitch*PI/180));
    front.setY(sin(Pitch*PI/180.0));
    front.setZ(sin(Yaw*PI/180)* cos(Pitch*PI/180));
    Front = front;
    Right = QVector3D::crossProduct(Front, WorldUp);
    Right.normalize();
    Up = QVector3D::crossProduct(Right, Front);
    Up.normalize();
}

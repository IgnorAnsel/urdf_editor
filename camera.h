#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>
#include <vector>
#include <QOpenGLFunctions_4_5_Core>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 1.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 70.0f;

class Camera
{
public:
    QVector3D Position;
    QVector3D Front;
    QVector3D Up;
    QVector3D Right;
    QVector3D WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    Camera(QVector3D postion = QVector3D(10.0f, 10.0f, 10.0f), QVector3D up = QVector3D(0.0f, 0.0f, 1.0f), float yaw = -135.0f, float pitch = 328.0f);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
    QVector3D GetForwardVector();
    QVector3D GetUpVector();
    QVector3D GetRightVector();
    QMatrix4x4 GetViewMatrix();
    QMatrix4x4 GetProjectionMatrix(float Ratio);
    void ProcessKeyborad(Camera_Movement direction, float deltaTime);
    void ProcessMouseMoveMent(float xoffset, float yoffset, GLboolean constrainPitch);
    void ProcessMouseScroll(float yoffset);
private:
    void updateCameraVectors();

};

#endif // CAMERA_H

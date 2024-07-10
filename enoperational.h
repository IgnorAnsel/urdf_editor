#ifndef ENOPERATIONAL_H
#define ENOPERATIONAL_H
#include "urdf_editor.h"
#include ""
#include <QObject>
#include <QKeyEvent>



class enOperational
{
    Q_OBJECT
public:
    enOperational();
protected:
    void keyPressEvent(QKeyEvent *event); //键盘按下事件
    void keyReleaseEvent(QKeyEvent *event); //键盘松开事件
};

#endif // ENOPERATIONAL_H

#ifndef CODEMAKER_H
#define CODEMAKER_H
#include "urdf_editor.h"
#include <QObject>
#include <QFile>
#include <QDir>
#include <QUuid>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
class codemaker : public QObject
{
    Q_OBJECT
public:
    explicit codemaker(QObject *parent = nullptr);
    void CodeMake(QString path,std::vector<Shape> shapes);
private:
    QString path;
signals:

};

#endif // CODEMAKER_H

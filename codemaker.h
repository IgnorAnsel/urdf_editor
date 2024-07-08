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
    void CodeBegin(QString path,QString name);
    void CodeMake(QString path,std::vector<Shape> shapes);
    std::vector<Shape> Loader(QString path);
    std::vector<Shape> Loader_joint(std::vector<Shape> shapes,QString path);
private:
    QString path;
    bool isOriginNonZero(const QVector3D& position, const QVector3D& rotation);
    bool isInertiaMatrixNonZero(const Inertia& matrix);

};

#endif // CODEMAKER_H

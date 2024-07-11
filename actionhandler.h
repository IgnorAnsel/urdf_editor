#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QInputDialog>
#include "codemaker.h"
class ActionHandler : public QWidget
{
    Q_OBJECT
public:
    explicit ActionHandler(QWidget *parent = nullptr) : QWidget(parent) {}
    QString savedFilePath;
    codemaker *cm;
signals:
    void set_selectedShapeIndex();
    void changestatus(QString path);
    void clear();
    void update(std::vector<Shape> shapes);
private:
public slots:
    /****************************** 文件菜单 **********************************/
    void newFile();    // 新建文件
    void openFile();   // 打开文件
    void saveFile();   // 保存文件
    void saveasFile(); // 另存文件
    void Exit();       // 退出

    ///****************************** 编辑菜单 **********************************/
    //    void undo();// 撤销
    //    void redo();// 重做
    //    void cut();// 剪切
    //    void copy();// 复制
    //    void paste();// 粘贴

    ///****************************** 视图菜单 **********************************/
    //    void zoom_in();// 放大
    //    void zoom_out();// 缩小
    //    void reset_view();// 正常

    ///****************************** 插入菜单 **********************************/
    //    void add_link();// 插入链接
    //    void add_joint();// 插入关节

    ///****************************** 工具菜单 **********************************/
    //    void validate_model();// 验证
    //    void export_model();// 导出模型

    ///****************************** 帮助菜单 **********************************/
    //    void document();// 帮助文档
    //    void about();// 关于


    };
#endif // ACTIONHANDLER_H

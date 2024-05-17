#include "actionhandler.h"
void ActionHandler::newFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                 "/home/",
                                                 tr("Urdf Files (*.urdf)"));
    if (fileName.isEmpty())
       return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) 
    {
       QMessageBox::warning(this, tr("Application"),tr("Create failed"));
    }
}
void ActionHandler::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 "/home/",
                                                 tr("Text Files (*.urdf)"));
    if (fileName.isEmpty())
       return;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) 
    {
       QMessageBox::warning(this, tr("Application"),tr("Open failed"));
    }
}
void ActionHandler::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                 "/home/",
                                                 tr("Text Files (*.urdf)"));
    if (fileName.isEmpty())
       return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) 
    {
       QMessageBox::warning(this, tr("Application"),tr("Save failed"));
    }
}
void ActionHandler::saveasFile()
{
    //另存为文件
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                 "/home/",
                                                 tr("Text Files (*.urdf)"));
    if (fileName.isEmpty())
       return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) 
    {
       QMessageBox::warning(this, tr("Application"),tr("Save failed"));
    }
}
void ActionHandler::Exit()
{
    //退出
    QApplication::quit();
}


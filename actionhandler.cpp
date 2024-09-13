#include "actionhandler.h"
void ActionHandler::newFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                    "/home/",
                                                    tr("Urdf Files (*.urdf)"));
    if (fileName.isEmpty())
        return;

    // 检查文件名是否已经有 .urdf 扩展名，如果没有则添加
    if (!fileName.endsWith(".urdf", Qt::CaseInsensitive))
    {
        fileName += ".urdf";
    }
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Application"), tr("Create failed"));
    }
    // 弹出输入对话框，获取 robotname
    bool ok;
    QString robotName = QInputDialog::getText(this, tr("Input Robot Name"),
                                              tr("Robot Name:"), QLineEdit::Normal,
                                              "", &ok);
    if (!ok || robotName.isEmpty())
        return;
    cm->CodeBegin(fileName,robotName);
    savedFilePath = fileName;
    emit clear();
    changestatus(savedFilePath);
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
      QMessageBox::warning(this, tr("Application"), tr("Open failed"));
   }
   savedFilePath = fileName;
   //emit clear();
   changestatus(savedFilePath);
   qDebug() << "1";
   emit update(cm->Loader(savedFilePath));
}
void ActionHandler::saveasFile()
{
   emit set_selectedShapeIndex();
   auto &shapes = shapeManager.getShapes();
   // 另存为文件
   QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                   "/home/",
                                                   tr("Text Files (*.urdf)"));
   if (fileName.isEmpty())
      return;
   cm->CodeMake(fileName,shapes);
}
void ActionHandler::saveFile()
{
   emit set_selectedShapeIndex();
   auto &shapes = shapeManager.getShapes();
   QString fileName = savedFilePath;
   if (fileName.isEmpty())
      return;
   QFile file(fileName);
   if (!file.open(QFile::WriteOnly | QFile::Text))
   {
      QMessageBox::warning(this, tr("Application"), tr("Save failed"));
   }
   cm->CodeMake(fileName,shapes);
   changestatus(savedFilePath);
}
void ActionHandler::Exit()
{
   // 退出
   QApplication::quit();
}

void ActionHandler::b_setting()
{
   bs->exec();
}

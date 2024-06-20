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
      QMessageBox::warning(this, tr("Application"), tr("Create failed"));
   }
   savedFilePath = fileName;
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
}
void ActionHandler::saveFile()
{
   emit set_selectedShapeIndex();
   QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                   "/home/",
                                                   tr("Text Files (*.urdf)"));
   if (fileName.isEmpty())
      return;
   cm->CodeMake(fileName,shapes);
//   QFile file(fileName);
//   if (!file.open(QFile::WriteOnly | QFile::Text))
//   {
//      QMessageBox::warning(this, tr("Application"), tr("Save failed"));
//   }
}
void ActionHandler::saveasFile()
{
   emit set_selectedShapeIndex();
   // 另存为文件
   QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                   "/home/",
                                                   tr("Text Files (*.urdf)"));
   if (fileName.isEmpty())
      return;
   QFile file(fileName);
   if (!file.open(QFile::WriteOnly | QFile::Text))
   {
      QMessageBox::warning(this, tr("Application"), tr("Save failed"));
   }
}
void ActionHandler::Exit()
{
   // 退出
   QApplication::quit();
}

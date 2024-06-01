#ifndef SHAPE_RELATION_H
#define SHAPE_RELATION_H

#include <QWidget>
#include "urdf_editor.h"
#include <QTreeWidget>
namespace Ui
{
    class shape_relation;
}

class shape_relation : public QWidget
{
    Q_OBJECT

public:
    explicit shape_relation(QWidget *parent = nullptr);
    void addShapesToTreeWidget(const std::vector<Shape> &shapes, QTreeWidget *treeWidget);
    ~shape_relation();
public slots:
    void update_shape();
private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

signals:
    void updateInde(int index);
protected:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;


private:
    int num =0;
    Ui::shape_relation *ui;
    QStringList getChildItemNames(QTreeWidgetItem* item);
    QTreeWidgetItem* findRemovedParentItem(QTreeWidgetItem* parentItem, const QString& removedChildName);
    void updateJointNames(QTreeWidgetItem* item, const QString& parentPath);
};

#endif // SHAPE_RELATION_H

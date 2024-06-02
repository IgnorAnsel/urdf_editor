#ifndef SHAPE_RELATION_H
#define SHAPE_RELATION_H

#include <QWidget>
#include "urdf_editor.h"
#include <QTreeWidget>
#include <set>
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
    void updateJointIndex(int index);
    void uptatepaste();

protected:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    int num = 0;
    int joint_num = 0;
    Ui::shape_relation *ui;
    Shape copiedShape;
    int copiedId;
    QStringList getChildItemNames(QTreeWidgetItem *item);
    QTreeWidgetItem *findRemovedParentItem(QTreeWidgetItem *parentItem, const QString &removedChildName);
    void updateJointNames(QTreeWidgetItem *item, const QString &parentPath);
    bool findAndUpdateNode(QTreeWidgetItem *parent, int id, const QString &name);
    void removeInvalidNodes(QTreeWidgetItem *parent, const std::set<int> &shapeIds, const std::map<int, std::string> &shapeNameMap);
    void copyItem();
    void pasteItem();
    void updateShapeIds(QTreeWidgetItem *root);
    void recursiveUpdateShapeIds(QTreeWidgetItem *node, int parentId);
};

#endif // SHAPE_RELATION_H

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
signals:
    void updateInde(int index);

private:
    Ui::shape_relation *ui;
};

#endif // SHAPE_RELATION_H

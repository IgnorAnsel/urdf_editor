#ifndef COLLAPSIBLEWIDGET_H
#define COLLAPSIBLEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
namespace Ui {
class CollapsibleWidget;
}
class CollapsibleWidget : public QWidget {
    Q_OBJECT
public:
    explicit CollapsibleWidget(const QString &title, QWidget *parent = nullptr);
    ~CollapsibleWidget();

private slots:
    void toggleContentVisibility();

private:
    QWidget *contentWidget;
    Ui::CollapsibleWidget *ui;
};

#endif // COLLAPSIBLEWIDGET_H


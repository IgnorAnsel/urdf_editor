#ifndef GEOMETRY_CHOOSE_H
#define GEOMETRY_CHOOSE_H

#include <QWidget>

namespace Ui {
class geometry_choose;
}

class geometry_choose : public QWidget
{
    Q_OBJECT

public:
    explicit geometry_choose(QWidget *parent = nullptr);
    ~geometry_choose();

private:
    Ui::geometry_choose *ui;
};

#endif // GEOMETRY_CHOOSE_H

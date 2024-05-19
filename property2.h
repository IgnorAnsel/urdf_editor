#ifndef PROPERTY2_H
#define PROPERTY2_H

#include <QWidget>

namespace Ui {
class property2;
}

class property2 : public QWidget
{
    Q_OBJECT

public:
    explicit property2(QWidget *parent = nullptr);
    ~property2();

private:
    Ui::property2 *ui;
};

#endif // PROPERTY2_H

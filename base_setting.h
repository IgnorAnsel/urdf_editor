#ifndef BASE_SETTING_H
#define BASE_SETTING_H

#include <QDialog>

namespace Ui {
class base_setting;
}

class base_setting : public QDialog
{
    Q_OBJECT

public:
    explicit base_setting(QWidget *parent = nullptr);
    ~base_setting();

private slots:
    void on_comboBox_currentTextChanged(const QString &arg1);
signals:
    void choose_drak_light(int dl);
private:
    Ui::base_setting *ui;
};

#endif // BASE_SETTING_H

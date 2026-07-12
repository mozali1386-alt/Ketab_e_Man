#ifndef USERSIGNUPWINDOW_H
#define USERSIGNUPWINDOW_H

#include <QMainWindow>

namespace Ui {
class Usersignupwindow;
}

class Usersignupwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Usersignupwindow(QWidget *parent = nullptr);
    ~Usersignupwindow();

private:
    Ui::Usersignupwindow *ui;
};

#endif // USERSIGNUPWINDOW_H

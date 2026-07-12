#ifndef SIGNINWINDOW_H
#define SIGNINWINDOW_H

#include <QMainWindow>

namespace Ui {
class signinwindow;
}

class signinwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit signinwindow(QWidget *parent = nullptr);
    ~signinwindow();

private:
    Ui::signinwindow *ui;
};

#endif // SIGNINWINDOW_H

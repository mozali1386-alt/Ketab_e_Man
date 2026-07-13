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
    explicit signinwindow(const QString &role, QWidget *parent = nullptr);
    ~signinwindow();

signals:
    void backtologinwindow();
    void signinsuccessful();

private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_Confirm_clicked();

    void on_pushButton_forgetpassword_clicked();

private:
    Ui::signinwindow *ui;
    QString role;
};

#endif // SIGNINWINDOW_H

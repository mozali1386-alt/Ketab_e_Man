#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QMainWindow>

namespace Ui {
class UserDashboard;
}

class UserDashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserDashboard(QWidget *parent = nullptr);
    ~UserDashboard();

private slots:
    void on_pushButton_store_clicked();

    void on_pushButton_cart_clicked();

    void on_pushButton_mylibrary_clicked();

    void on_pushButton_profile_clicked();

private:
    Ui::UserDashboard *ui;
};

#endif // USERDASHBOARD_H

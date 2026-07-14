#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QMessageBox>
#include "publishersignupwindow.h"
#include "signinwindow.h"
#include "usersignupwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_pushButton_signup_clicked()
{
    QString role = ui->comboBox_role->currentText().trimmed();

    if (role == "مدیر") {
        // نمایش ارور با استفاده از QMessageBox
        QMessageBox::warning(this, "خطا", "برای مدیر گزینه ثبت نام وجود ندارد");

    } else if (role == "ناشر") {
        publishersignupwindow *publishersignup = new publishersignupwindow();
        publishersignup->show();

        connect(publishersignup,
                &publishersignupwindow::backtologinwindow,
                this,
                &LoginWindow::show);
        connect(publishersignup,
                &publishersignupwindow::signupsuccessful,
                this,
                &LoginWindow::close);

        this->hide();

    } else if (role == "کاربر عادی") {
        Usersignupwindow *usersignup = new Usersignupwindow();
        usersignup->show();
        this->hide();

        connect(usersignup, &Usersignupwindow::backtologinwindow, this, &LoginWindow::show);
        connect(usersignup, &Usersignupwindow::signupsuccessful, this, &LoginWindow::close);
    }
}

void LoginWindow::on_pushButton_signin_clicked()
{
    QString role = ui->comboBox_role->currentText().trimmed();
    QString englishrole;
    if (role == "مدیر")
        englishrole = "Admin";
    if (role == "کاربر عادی")
        englishrole = "Normaluser";
    if (role == "ناشر")
        englishrole = "Publisher";

    signinwindow *signin = new signinwindow(englishrole);

    signin->show();
    this->hide();

    connect(signin, &signinwindow::backtologinwindow, this, &LoginWindow::show);
    connect(signin, &signinwindow::signinsuccessful, this, &LoginWindow::close);
}

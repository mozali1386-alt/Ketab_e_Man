#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QMessageBox>
#include "publishersignupwindow.h"
#include "signinwindow.h"
#include "usersignupwindow.h"

LoginWindow::LoginWindow(ClientSocketManager *client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , m_client(client)
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
        QMessageBox::warning(this, "خطا", "برای مدیر گزینه ثبت نام وجود ندارد");

    } else if (role == "ناشر") {
        publishersignupwindow *publishersignup = new publishersignupwindow(m_client);
        publishersignup->show();
        this->hide();

        connect(publishersignup,
                &publishersignupwindow::backtologinwindow,
                this,
                &LoginWindow::show);

        connect(publishersignup, &publishersignupwindow::signupsuccessful, this, [=]() {
            signinwindow *signin = new signinwindow(m_client, "PUBLISHER");
            signin->show();
            connect(signin, &signinwindow::backtologinwindow, this, &LoginWindow::show);
            connect(signin, &signinwindow::signinsuccessful, this, &LoginWindow::close);
        });

    } else if (role == "کاربر عادی") {
        Usersignupwindow *usersignup = new Usersignupwindow(m_client);
        usersignup->show();
        this->hide();

        connect(usersignup, &Usersignupwindow::backtologinwindow, this, &LoginWindow::show);

        connect(usersignup, &Usersignupwindow::signupsuccessful, this, [=]() {
            signinwindow *signin = new signinwindow(m_client, "NORMALUSER");
            signin->show();
            connect(signin, &signinwindow::backtologinwindow, this, &LoginWindow::show);
            connect(signin, &signinwindow::signinsuccessful, this, &LoginWindow::close);
        });
    }
}

void LoginWindow::on_pushButton_signin_clicked()
{
    QString role = ui->comboBox_role->currentText().trimmed();
    QString englishrole;
    if (role == "مدیر")
        englishrole = "ADMIN";
    if (role == "کاربر عادی")
        englishrole = "NORMALUSER";
    if (role == "ناشر")
        englishrole = "PUBLISHER";

    signinwindow *signin = new signinwindow(m_client, englishrole);

    signin->show();
    this->hide();

    connect(signin, &signinwindow::backtologinwindow, this, &LoginWindow::show);
    connect(signin, &signinwindow::signinsuccessful, this, &LoginWindow::close);
}
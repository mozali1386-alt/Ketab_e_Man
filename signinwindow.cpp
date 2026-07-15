#include "signinwindow.h"
#include "admindashboard.h"
#include "publisherdashboard.h"
#include "resetpassworddialog.h"
#include "ui_signinwindow.h"
#include "userdashboard.h"
signinwindow::signinwindow(const QString &role, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::signinwindow)
    , role(role)
{
    ui->setupUi(this);
    ui->label_usernameError->setStyleSheet("color: red;");
    ui->label_passwordError->setStyleSheet("color: red;");

    // حتماً این خط را اضافه کن تا با close شدن، صفحه ورود از حافظه پاک شود
    this->setAttribute(Qt::WA_DeleteOnClose);
}

signinwindow::~signinwindow()
{
    delete ui;
}
void signinwindow::on_pushButton_back_clicked()
{
    emit backtologinwindow();
    this->close();
}

void signinwindow::on_pushButton_Confirm_clicked()
{
    ui->label_usernameError->clear();
    ui->label_passwordError->clear();

    QString username = ui->lineEdit_UserName->text().trimmed();
    QString password = ui->lineEdit_Password->text().trimmed();

    bool hasError = false; // متغیری برای اینکه بدانیم اصلاً خطایی داریم یا نه

    //  بررسی خالی بودن نام کاربری
    if (username.isEmpty()) {
        ui->label_usernameError->setText("نام کاربری نمی تواند خالی باشد.");
        hasError = true;
    }

    //  بررسی خالی بودن رمز عبور
    if (password.isEmpty()) {
        ui->label_passwordError->setText("رمز عبور نمی تواند خالی باشد.");
        hasError = true;
    }

    if (hasError) {
        return;
    }

    QString Message = QString("LOGIN||%1||%2||%3").arg(role, username, password);

    qDebug() << Message;
    //client->sendMessage(Message);

    QMainWindow *dash = nullptr;
    //بعدا که سرور درست شد باید جواب سرور را به جای role قرار دهیم
    if (role == "ADMIN")
        dash = new Admindashboard();

    else if (role == "NORMALUSER")
        dash = new UserDashboard();

    else if (role == "PUBLISHER")
        dash = new Publisherdashboard();

    dash->show();
    emit signinsuccessful();
    this->close();
}

void signinwindow::on_pushButton_forgetpassword_clicked()
{
    ResetPasswordDialog dialog(this);
    // استفاده از exec برای باز شدن پنجره در حالت modal

    dialog.exec();
}

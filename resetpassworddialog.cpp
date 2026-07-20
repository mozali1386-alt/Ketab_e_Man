#include "resetpassworddialog.h"
#include <QRegularExpression>
#include "ui_resetpassworddialog.h"

ResetPasswordDialog::ResetPasswordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetPasswordDialog)
{
    ui->setupUi(this);

    QString errorStyle = "color: red; background-color: transparent; border: none;";
    ui->label_passworderror->setStyleSheet(errorStyle);
    ui->label_repeatpassworderror->setStyleSheet(errorStyle);
    ui->label_EmailError->setStyleSheet(errorStyle);
    ui->stackedWidget->setCurrentIndex(0);
}

ResetPasswordDialog::~ResetPasswordDialog()
{
    delete ui;
}

void ResetPasswordDialog::on_pushButton_page1back_clicked()
{
    this->close();
}

void ResetPasswordDialog::on_pushButton_page1next_clicked()
{
    ui->label_EmailError->clear();
    QString email = ui->lineEdit_Email->text().trimmed();

    if (email.isEmpty()) {
        ui->label_EmailError->setText("لطفاً ایمیل خود را وارد کنید.");
        return;
    }

    QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailRegex.match(email).hasMatch()) {
        ui->label_EmailError->setText("فرمت ایمیل نامعتبر است.");
        return;
    }

    QString Message = QString("FORGOT_PASS_CHECK||%1").arg(email);
    qDebug() << Message;

    //client->sendMessage(Message);
    // ===================
    //جواب سرور
    //====================

    QString simulatedUsernameFromServer = "ali"; // فرضی برای نشان دادن نام کاربری

    ui->label_username->setText("کاربر  " + simulatedUsernameFromServer + " خوش آمدید");
    ui->stackedWidget->setCurrentIndex(1);
}

void ResetPasswordDialog::on_pushButton_page2back_2_clicked()
{
    if (isFromProfile) {
        this->reject(); // اگر از پروفایل بود، کل پنجره را ببند
    } else {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void ResetPasswordDialog::on_pushButton_page2next_2_clicked()
{
    ui->label_passworderror->clear();
    ui->label_repeatpassworderror->clear();

    QString newPassword = ui->lineEdit_password->text().trimmed();
    QString repeatPassword = ui->lineEdit_repeatpassword->text().trimmed();
    bool hasError = false;

    if (newPassword.isEmpty()) {
        ui->label_passworderror->setText("رمز عبور جدید را وارد کنید.");
        hasError = true;
    }

    if (repeatPassword.isEmpty()) {
        ui->label_repeatpassworderror->setText("لطفاً رمز عبور را تکرار کنید.");
        hasError = true;
    } else if (newPassword != repeatPassword) {
        ui->label_repeatpassworderror->setText("تکرار رمز عبور با رمز عبور جدید یکسان نیست.");
        hasError = true;
    }

    if (hasError) {
        return;
    }

    QString Message = QString("FORGOT_PASS_UPDATE||%1").arg(newPassword);
    qDebug() << Message;

    //client->sendMessage(Message);
    // ===================
    //جواب سرور
    //====================

    this->accept();
}

void ResetPasswordDialog::setupForProfile()
{
    isFromProfile = true;
    ui->stackedWidget->setCurrentIndex(1);
}
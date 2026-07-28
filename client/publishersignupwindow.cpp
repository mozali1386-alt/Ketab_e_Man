#include "publishersignupwindow.h"
#include <QRegularExpression>
#include "publisherdashboard.h"
#include "ui_publishersignupwindow.h"

publishersignupwindow::publishersignupwindow(ClientSocketManager *client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::publishersignupwindow)
    , m_client(client)
{
    ui->setupUi(this);
    connect(m_client,
            &ClientSocketManager::messageReceived,
            this,
            &publishersignupwindow::processServerResponse);
    ui->label_errornashr->setStyleSheet("color: red;");
    ui->label_errorusername->setStyleSheet("color: red;");
    ui->label_erroremail->setStyleSheet("color: red;");
    ui->label_errorpassword->setStyleSheet("color: red;");
    ui->label_errorrepeatpassword->setStyleSheet("color: red;");

    this->setAttribute(Qt::WA_DeleteOnClose);
}

publishersignupwindow::~publishersignupwindow()
{
    delete ui;
}

void publishersignupwindow::on_pushButton_back_clicked()
{
    emit backtologinwindow();
    this->close();
}

void publishersignupwindow::on_pushButton_confirm_clicked()
{
    ui->label_errornashr->clear();
    ui->label_errorusername->clear();
    ui->label_erroremail->clear();
    ui->label_errorpassword->clear();
    ui->label_errorrepeatpassword->clear();

    QString publisher = ui->lineEdit_nashr->text().trimmed();
    QString username = ui->lineEdit_username->text().trimmed();
    QString email = ui->lineEdit_Email->text().trimmed();
    QString password = ui->lineEdit_password->text().trimmed();
    QString confirmPassword = ui->lineEdit_repeatpassword->text().trimmed();

    bool hasError = false;

    if (publisher.isEmpty()) {
        ui->label_errornashr->setText("نام انتشارات نمی تواند خالی باشد");
        hasError = true;
    }

    if (username.isEmpty()) {
        ui->label_errorusername->setText("نام کاربری نمی تواند خالی باشد.");
        hasError = true;
    }

    if (email.isEmpty()) {
        ui->label_erroremail->setText("ایمیل نمی تواند خالی باشد.");
        hasError = true;
    } else {
        QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
        QRegularExpressionMatch match = emailRegex.match(email);

        if (!match.hasMatch()) {
            ui->label_erroremail->setText("فرمت ایمیل نامعتبر است.");
            hasError = true;
        }
    }

    if (password.isEmpty()) {
        ui->label_errorpassword->setText("رمز عبور نمی تواند خالی باشد.");
        hasError = true;
    }

    if (confirmPassword.isEmpty()) {
        ui->label_errorrepeatpassword->setText("لطفاً رمز عبور را تکرار کنید.");
        hasError = true;
    } else if (password != confirmPassword) {
        ui->label_errorrepeatpassword->setText("رمز عبور و تکرار آن یکسان نیستند.");
        hasError = true;
    }

    if (hasError) {
        return;
    }

    QString Message = QString("SIGNUP_PUBLISHER||%1||%2||%3||%4")
                          .arg(publisher, username, email, password);
    qDebug() << Message;

    m_client->sendMessage(Message);

    // ======== کدهای تست ثبت‌نام ناشر ========
    // تست حالت موفقیت‌آمیز:
    //processServerResponse("SIGNUP_PUBLISHER||SUCCESS");

    // تست حالت خطای دوتایی:
    //processServerResponse("SIGNUP_PUBLISHER||FAIL||USERNAME_EXISTS,EMAIL_EXISTS");
}

void publishersignupwindow::processServerResponse(const QString &response)
{
    QStringList parts = response.split("||");
    if (parts.isEmpty())
        return;

    if (parts[0] == "SIGNUP_PUBLISHER") {
        if (parts.size() >= 2 && parts[1] == "SUCCESS") {
            Publisherdashboard *dash = new Publisherdashboard(m_client);
            dash->show();
            emit signupsuccessful();
            this->close();
        } else if (parts.size() >= 3 && parts[1] == "FAIL") {
            QStringList errors = parts[2].split(",");
            for (int i = 0; i < errors.size(); ++i) {
                if (errors[i] == "USERNAME_EXISTS") {
                    ui->label_errorusername->setText("این نام کاربری از قبل وجود دارد.");
                } else if (errors[i] == "EMAIL_EXISTS") {
                    ui->label_erroremail->setText("این ایمیل از قبل ثبت شده است.");
                }
            }
        }
    }
}

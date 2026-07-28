#include "signinwindow.h"
#include "admindashboard.h"
#include "publisherdashboard.h"
#include "resetpassworddialog.h"
#include "ui_signinwindow.h"
#include "userdashboard.h"
signinwindow::signinwindow(ClientSocketManager *client, const QString &role, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::signinwindow)
    , role(role)
    , m_client(client)
{
    ui->setupUi(this);
    connect(m_client,
            &ClientSocketManager::messageReceived,
            this,
            &signinwindow::processServerResponse);

    ui->label_usernameError->setStyleSheet("color: red;");
    ui->label_passwordError->setStyleSheet("color: red;");

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

    bool hasError = false;

    if (username.isEmpty()) {
        ui->label_usernameError->setText("نام کاربری نمی تواند خالی باشد.");
        hasError = true;
    }

    if (password.isEmpty()) {
        ui->label_passwordError->setText("رمز عبور نمی تواند خالی باشد.");
        hasError = true;
    }

    if (hasError) {
        return;
    }

    QString Message = QString("LOGIN||%1||%2||%3").arg(role, username, password);

    qDebug() << Message;
    m_client->sendMessage(Message);
}

void signinwindow::on_pushButton_forgetpassword_clicked()
{
    ResetPasswordDialog dialog(m_client, this);
    dialog.exec();
}

void signinwindow::processServerResponse(const QString &response)
{
    QStringList parts = response.split("||");
    if (parts.isEmpty())
        return;

    if (parts[0] == "LOGIN") {
        if (parts.size() >= 2 && parts[1] == "SUCCESS") {
            QMainWindow *dash = nullptr;
            if (role == "ADMIN")
                dash = new Admindashboard(m_client);
            else if (role == "NORMALUSER")
                dash = new UserDashboard(m_client);
            else if (role == "PUBLISHER")
                dash = new Publisherdashboard(m_client);

            if (dash != nullptr) {
                dash->show();
                emit signinsuccessful();
                this->close();
            }
        } else if (parts.size() >= 3 && parts[1] == "FAIL") {
            // دریافت متن خطای انگلیسی از سرور
            QString serverError = parts[2].trimmed();
            QString persianError;
            if (serverError == "Invalid username or password") {
                persianError = "نام کاربری یا رمز عبور اشتباه است.";
            } else if (serverError == "Role mismatch" || serverError == "Invalid role") {
                persianError = "نقش انتخاب شده با حساب کاربری شما مطابقت ندارد.";
            } else {
                persianError = "خطایی در ورود رخ داد. لطفاً دوباره تلاش کنید.";
            }
            ui->label_passwordError->setText(persianError);
        }
    }
}
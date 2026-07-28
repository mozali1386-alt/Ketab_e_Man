#include "publisherprofilepage.h"
#include <QMessageBox>
#include <QRegularExpression>
#include "resetpassworddialog.h"
#include "ui_publisherprofilepage.h"

PublisherProfilePage::PublisherProfilePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PublisherProfilePage)
{
    ui->setupUi(this);

    ui->label_error_entesharat->setStyleSheet("color: red;");
    ui->label_error_email->setStyleSheet("color: red;");
    ui->label_error_entesharat->clear();
    ui->label_error_email->clear();
}

PublisherProfilePage::~PublisherProfilePage()
{
    delete ui;
}

void PublisherProfilePage::on_pushButton_save_clicked()
{
    ui->label_error_entesharat->clear();
    ui->label_error_email->clear();

    QString currentEntesharat = ui->lineEdit_entesharat->text().trimmed();
    QString currentEmail = ui->lineEdit_email->text().trimmed();
    QString currentBio = ui->textEdit_explanation->toPlainText().trimmed();
    bool hasError = false;

    if (currentEntesharat.isEmpty()) {
        ui->label_error_entesharat->setText(
            "نام انتشارات نمی‌تواند خالی باشد.");
        hasError = true;
    }

    if (currentEmail.isEmpty()) {
        ui->label_error_email->setText("ایمیل نمی‌تواند خالی باشد.");
        hasError = true;
    } else {
        QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
        if (!emailRegex.match(currentEmail).hasMatch()) {
            ui->label_error_email->setText("فرمت ایمیل نامعتبر است.");
            hasError = true;
        }
    }

    if (hasError
        || (currentEntesharat == originalEntesharat && currentEmail == originalEmail
            && currentBio == originalExplanation))
        return;

    QString message = QString("UPDATE_PUB_PROFILE||%1||%2||%3")
                          .arg(currentEntesharat, currentEmail, currentBio);
    m_client->sendMessage(message);
}

void PublisherProfilePage::on_pushButton_enseraf_clicked()
{
    ui->label_error_entesharat->clear();
    ui->label_error_email->clear();

    ui->lineEdit_entesharat->setText(originalEntesharat);
    ui->lineEdit_email->setText(originalEmail);
    ui->textEdit_explanation->setText(originalExplanation);
}

void PublisherProfilePage::on_pushButton_withdrawbalance_clicked()
{
    QString currentText = ui->label_balance->text();
    currentText.replace(" تومان", "");

    if (currentText.toInt() <= 0) {
        QMessageBox::warning(this, "خطا", "موجودی شما کافی نیست.");
        return;
    }
    m_client->sendMessage("WITHDRAW_BALANCE");
}

void PublisherProfilePage::on_pushButton_editusername_clicked()
{
    ResetPasswordDialog dialog(m_client, this);
    dialog.setupForProfile();
    dialog.exec();
}

void PublisherProfilePage::processServerResponse(const QString &response)
{
    QStringList parts = response.split("||");
    if (parts.size() < 2)
        return;

    QString cmd = parts[0].trimmed();
    QString param = parts[1].trimmed();

    if (cmd == "PUB_PROFILE_INFO" && parts.size() >= 5) {
        originalEntesharat = parts[1].trimmed();
        originalEmail = parts[3].trimmed();

        ui->lineEdit_entesharat->setText(originalEntesharat);
        ui->lineEdit_username->setText(parts[2].trimmed());
        ui->lineEdit_email->setText(originalEmail);
        ui->label_balance->setText(parts[4].trimmed() + " تومان");

        if (parts.size() >= 6) {
            originalExplanation = parts[5].trimmed();
        } else {
            originalExplanation = "";
        }

        ui->textEdit_explanation->setText(originalExplanation);
    } else if (cmd == "UPDATE_PUB_PROFILE_RESULT") {
        if (param == "SUCCESS") {
            originalEntesharat = ui->lineEdit_entesharat->text().trimmed();
            originalEmail = ui->lineEdit_email->text().trimmed();
            originalExplanation = ui->textEdit_explanation->toPlainText().trimmed();
            QMessageBox::information(this, "موفقیت", "تغییرات ثبت شد.");
        } else {
            if (param.contains("DUPLICATE_ENTESHARAT")) {
                ui->label_error_entesharat->setText("این نام انتشارات قبلاً ثبت شده است.");
            }

            if (param.contains("DUPLICATE_EMAIL")) {
                ui->label_error_email->setText("این ایمیل قبلاً ثبت شده است.");
            }
        }
    } else if (cmd == "WITHDRAW_BALANCE_RESULT" && param == "SUCCESS") {
        ui->label_balance->setText("0 تومان");
        QMessageBox::information(this, "موفقیت", "مبلغ برداشت شد.");
    }
}
void PublisherProfilePage::refreshProfile()
{
    m_client->sendMessage("GET_PUB_PROFILE_INFO");
}
void PublisherProfilePage::setClient(ClientSocketManager *client)
{
    m_client = client;
    connect(m_client,
            &ClientSocketManager::messageReceived,
            this,
            &PublisherProfilePage::processServerResponse);
}
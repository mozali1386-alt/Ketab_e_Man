#include "bookmanagement.h"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QIcon>
#include <QMessageBox>
#include <QPixmap>
#include <QStringList>
#include "ui_bookmanagement.h"

BookManagement::BookManagement(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookManagement)
{
    ui->setupUi(this);

    // در ابتدا فرم روی حالت "افزودن کتاب" ریست می‌شود
    resetForm();

    // درخواست اولیه از سرور برای گرفتن لیست کتاب‌های این ناشر
    // client->sendMessage("GET_PUBLISHER_BOOKS");
}

BookManagement::~BookManagement()
{
    delete ui;
}

// ==========================================
// پردازش پیام‌های دریافتی از سرور (منطق اصلی شبکه)
// ==========================================
void BookManagement::processServerResponse(const QString &message)
{
    QStringList parts = message.split("||");
    if (parts.isEmpty())
        return;

    QString cmd = parts[0];

    if (cmd == "PUBLISHER_BOOKS_RESULT") {
        ui->comboBox_tipe->blockSignals(true);
        ui->comboBox_tipe->clear();
        ui->comboBox_tipe->addItem("افزودن کتاب", "NEW");

        if (parts.size() >= 3 && !parts[2].isEmpty()) {
            QStringList books = parts[2].split(",");
            for (const QString &b : books) {
                QStringList info = b.split(":");
                if (info.size() >= 2) {
                    ui->comboBox_tipe->addItem(info[1], info[0]);
                }
            }
        }
        ui->comboBox_tipe->blockSignals(false);
    }

    else if (cmd == "BOOK_DETAILS_RESULT") {
        if (parts.size() >= 10) {
            ui->lineEdit_bookname->setText(parts[2]);
            ui->lineEdit_anuturename->setText(parts[3]);
            ui->comboBox_genre->setCurrentText(parts[4]);
            ui->spinBox_price->setValue(parts[5].toInt());
            ui->spinBox_discount->setValue(parts[6].toInt());
            ui->textEdit_explanation->setText(parts[7]);

            QString status = parts[8];
            if (status == "1" || status.toUpper() == "TRUE") {
                ui->pushButton_disable->show();
                ui->pushButton_enable->hide();
            } else {
                ui->pushButton_disable->hide();
                ui->pushButton_enable->show();
            }

            QString imgBase64 = parts[9];
            if (imgBase64 != "EMPTY" && !imgBase64.isEmpty()) {
                QByteArray imgData = QByteArray::fromBase64(imgBase64.toLatin1());
                QPixmap coverPixmap;
                coverPixmap.loadFromData(imgData);
                if (!coverPixmap.isNull()) {
                    QPixmap scaled = coverPixmap.scaled(ui->pushButton_pikcher->size(),
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
                    ui->pushButton_pikcher->setText("");
                    ui->pushButton_pikcher->setIcon(QIcon(scaled));
                    ui->pushButton_pikcher->setIconSize(scaled.size());
                    m_base64Image = imgBase64;
                }
            }
            orig_bookName = ui->lineEdit_bookname->text();
            orig_authorName = ui->lineEdit_anuturename->text();
            orig_genre = ui->comboBox_genre->currentText();
            orig_price = ui->spinBox_price->value();
            orig_discount = ui->spinBox_discount->value();
            orig_explanation = ui->textEdit_explanation->toPlainText();
            orig_hasNewImage = false; // فعلا عکسی عوض نشده
        }
    } else if (cmd == "ADD_BOOK_RESULT") {
        if (parts.size() >= 3 && parts[1] == "SUCCESS") {
            QString generatedBookId = parts[2];
            sendPdfChunks(generatedBookId);
        }
    }
}

// ==========================================
// مدیریت رابط کاربری (تغییر وضعیت کمبوباکس)
// ==========================================
void BookManagement::on_comboBox_tipe_currentIndexChanged(int index)
{
    if (index < 0)
        return;

    if (index == 0) {
        currentSelectedBookId = "";
        resetForm();
    } else {
        currentSelectedBookId = ui->comboBox_tipe->currentData().toString();

        ui->pushButton_save->hide();
        ui->pushButton_edit->show();

        ui->pushButton_pdf->hide();
        ui->label_7->hide();

        ui->pushButton_pikcher->show();
        orig_hasNewImage = false;

        ui->lineEdit_bookname->clear();
        ui->lineEdit_anuturename->clear();
        ui->pushButton_pikcher->setText("در حال دریافت...");
        ui->pushButton_pikcher->setIcon(QIcon());

        // درخواست اطلاعات از سرور (پس از دریافت، تابع processServerResponse فراخوانی می‌شود)
        // client->sendMessage("GET_BOOK_DETAILS||" + currentSelectedBookId);
    }
}

void BookManagement::resetForm()
{
    ui->lineEdit_bookname->clear();
    ui->lineEdit_anuturename->clear();
    ui->spinBox_price->setValue(0);
    ui->spinBox_discount->setValue(0);
    ui->textEdit_explanation->clear();
    ui->comboBox_genre->setCurrentIndex(0);

    m_base64Image.clear();
    m_base64Pdf.clear();
    orig_hasNewImage = false;

    ui->pushButton_pikcher->setIcon(QIcon());
    ui->pushButton_pikcher->setText("انتخاب عکس");
    ui->pushButton_pdf->setText("انتخاب فایل پی دی اف");

    ui->pushButton_save->show();
    ui->pushButton_edit->hide();
    ui->pushButton_disable->hide();
    ui->pushButton_enable->hide();

    ui->pushButton_pdf->show();
    ui->label_7->show();

    ui->pushButton_pikcher->show();
}

// ==========================================
// چک کردن تغییرات فرم (برای جلوگیری از درخواست الکی)
// ==========================================
bool BookManagement::hasFormChanged()
{
    if (ui->lineEdit_bookname->text() != orig_bookName)
        return true;
    if (ui->lineEdit_anuturename->text() != orig_authorName)
        return true;
    if (ui->comboBox_genre->currentText() != orig_genre)
        return true;
    if (ui->spinBox_price->value() != orig_price)
        return true;
    if (ui->spinBox_discount->value() != orig_discount)
        return true;
    if (ui->textEdit_explanation->toPlainText() != orig_explanation)
        return true;
    if (orig_hasNewImage)
        return true;

    return false;
}

// ==========================================
// انتخاب فایل‌ها و مدیریت کیفیت عکس
// ==========================================
void BookManagement::setButtonImageFit(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    if (pixmap.isNull())
        return;

    QPixmap scaledPixmap = pixmap.scaled(ui->pushButton_pikcher->size(),
                                         Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation);

    ui->pushButton_pikcher->setText("");
    QIcon buttonIcon(scaledPixmap);
    ui->pushButton_pikcher->setIcon(buttonIcon);
    ui->pushButton_pikcher->setIconSize(scaledPixmap.size());
}

void BookManagement::on_pushButton_pikcher_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "انتخاب عکس روی جلد",
                                                    "",
                                                    "Images (*.png *.jpg *.jpeg)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            m_base64Image = QString::fromLatin1(file.readAll().toBase64());
            setButtonImageFit(filePath);
            orig_hasNewImage = true;
        }
    }
}

void BookManagement::on_pushButton_pdf_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "انتخاب فایل کتاب",
                                                    "",
                                                    "PDF Files (*.pdf)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly)) {
            m_base64Pdf = QString::fromLatin1(file.readAll().toBase64());
            QFileInfo fileInfo(filePath);
            ui->pushButton_pdf->setText("فایل انتخاب شد: " + fileInfo.fileName());
        }
    }
}

// ==========================================
// عملیات شبکه (ثبت، ویرایش و ارسال)
// ==========================================
void BookManagement::on_pushButton_save_clicked()
{
    if (ui->lineEdit_bookname->text().trimmed().isEmpty()
        || ui->lineEdit_anuturename->text().trimmed().isEmpty() || m_base64Pdf.isEmpty()
        || m_base64Image.isEmpty()) {
        QMessageBox::warning(this,
                             "خطا",
                             "لطفاً تمامی فیلدهای ضروری، عکس و فایل PDF را انتخاب کنید.");
        return;
    }

    QString msg = "ADD_BOOK_METADATA||" + ui->lineEdit_bookname->text() + "||"
                  + ui->lineEdit_anuturename->text() + "||" + ui->comboBox_genre->currentText()
                  + "||" + QString::number(ui->spinBox_price->value()) + "||"
                  + QString::number(ui->spinBox_discount->value()) + "||"
                  + ui->textEdit_explanation->toPlainText() + "||" + m_base64Image;

    // client->sendMessage(msg);
}

void BookManagement::sendPdfChunks(const QString &bookId)
{
    if (m_base64Pdf.isEmpty())
        return;

    int chunkSize = 32768;
    int totalChunks = (m_base64Pdf.length() + chunkSize - 1) / chunkSize;

    // client->sendMessage("UPLOAD_PDF_START||" + bookId + "||" + QString::number(totalChunks));

    for (int i = 0; i < totalChunks; ++i) {
        QString chunk = m_base64Pdf.mid(i * chunkSize, chunkSize);
        // client->sendMessage("UPLOAD_PDF_CHUNK||" + bookId + "||" + chunk);
    }

    // client->sendMessage("UPLOAD_PDF_END||" + bookId);

    QMessageBox::information(this, "موفق", "کتاب با موفقیت آپلود شد.");
    resetForm();
}

void BookManagement::on_pushButton_edit_clicked()
{
    if (ui->lineEdit_bookname->text().trimmed().isEmpty()
        || ui->lineEdit_anuturename->text().trimmed().isEmpty()) {
        QMessageBox::warning(
            this,
            "خطا",
            "نام کتاب و نام نویسنده نمی‌توانند خالی باشند.");
        return;
    }

    if (!hasFormChanged()) {
        QMessageBox::information(this,
                                 "بدون تغییر",
                                 "شما هیچ تغییری در اطلاعات کتاب ایجاد "
                                 "نکرده‌اید!");
        return;
    }

    QString imageToSend = orig_hasNewImage ? m_base64Image : "EMPTY";
    QString msg = "EDIT_BOOK||" + currentSelectedBookId + "||" + ui->lineEdit_bookname->text()
                  + "||" + ui->lineEdit_anuturename->text() + "||"
                  + ui->comboBox_genre->currentText() + "||"
                  + QString::number(ui->spinBox_price->value()) + "||"
                  + QString::number(ui->spinBox_discount->value()) + "||"
                  + ui->textEdit_explanation->toPlainText() + "||" + imageToSend;

    // client->sendMessage(msg);
    QMessageBox::information(this, "ارسال", "درخواست ویرایش به سرور ارسال شد.");

    // آپدیت کردن متغیرهای اصلی
    orig_bookName = ui->lineEdit_bookname->text();
    orig_authorName = ui->lineEdit_anuturename->text();
    orig_genre = ui->comboBox_genre->currentText();
    orig_price = ui->spinBox_price->value();
    orig_discount = ui->spinBox_discount->value();
    orig_explanation = ui->textEdit_explanation->toPlainText();
    orig_hasNewImage = false;
}

void BookManagement::on_pushButton_disable_clicked()
{
    // client->sendMessage("TOGGLE_BOOK_STATUS||" + currentSelectedBookId + "||DISABLE");
    ui->pushButton_disable->hide();
    ui->pushButton_enable->show();
}

void BookManagement::on_pushButton_enable_clicked()
{
    // client->sendMessage("TOGGLE_BOOK_STATUS||" + currentSelectedBookId + "||ENABLE");
    ui->pushButton_enable->hide();
    ui->pushButton_disable->show();
}
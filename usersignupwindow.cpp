#include "usersignupwindow.h"
#include <QRegularExpression>
#include "ui_usersignupwindow.h"
#include "userdashboard.h"

Usersignupwindow::Usersignupwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Usersignupwindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);

    // قرمز کردن تمام لیبل‌های خطا
    ui->label_NameError->setStyleSheet("color: red;");
    ui->label_UsernameError->setStyleSheet("color: red;");
    ui->label_EmailError->setStyleSheet("color: red;");
    ui->label_passwordError->setStyleSheet("color: red;");
    ui->label_repeatPasswordError->setStyleSheet("color: red;");
    ui->label_genreError->setStyleSheet("color: red;");
}

Usersignupwindow::~Usersignupwindow()
{
    delete ui;
}

void Usersignupwindow::on_pushButton_back_clicked()
{
    emit backtologinwindow();
    this->close();
}

void Usersignupwindow::on_pushButton_confirm_clicked()
{
    ui->label_NameError->clear();
    ui->label_UsernameError->clear();
    ui->label_EmailError->clear();
    ui->label_passwordError->clear();
    ui->label_repeatPasswordError->clear();
    ui->label_genreError->clear();

    bool hasError = false;

    QString name = ui->lineEdit_name->text().trimmed();
    QString username = ui->lineEdit_username->text().trimmed();
    QString email = ui->lineEdit_Email->text().trimmed();
    QString password = ui->lineEdit_password->text().trimmed();
    QString repeatPassword = ui->lineEdit_repeatpassword->text().trimmed();

    if (name.isEmpty()) {
        ui->label_NameError->setText("نام و نام خانوادگی نمی تواند خالی باشد.");
        hasError = true;
    }

    if (username.isEmpty()) {
        ui->label_UsernameError->setText("نام کاربری نمی تواند خالی باشد.");
        hasError = true;
    }

    if (email.isEmpty()) {
        ui->label_EmailError->setText("ایمیل نمی تواند خالی باشد.");
        hasError = true;
    } else {
        QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
        if (!emailRegex.match(email).hasMatch()) {
            ui->label_EmailError->setText("فرمت ایمیل نامعتبر است.");
            hasError = true;
        }
    }

    if (password.isEmpty()) {
        ui->label_passwordError->setText("رمز عبور نمی تواند خالی باشد.");
        hasError = true;
    }

    if (repeatPassword.isEmpty()) {
        ui->label_repeatPasswordError->setText("لطفاً رمز عبور را تکرار کنید.");
        hasError = true;
    } else if (password != repeatPassword) {
        ui->label_repeatPasswordError->setText("رمز عبور با تکرار آن یکسان نیستند.");
        hasError = true;
    }

    int checkedCount = 0;
    for (int i = 0; i < ui->listWidget_genre->count(); ++i) {
        if (ui->listWidget_genre->item(i)->checkState() == Qt::Checked) {
            checkedCount++;
        }
    }

    if (checkedCount == 0) {
        ui->label_genreError->setText("لطفاً حداقل ۱ ژانر را انتخاب کنید.");
        hasError = true;
    }

    if (hasError) {
        return;
    }
    QStringList englishgenre = {
        "ROMANCE", // آیتم 0: عاشقانه
        "CRIME",   // آیتم 1: جنایی
        "SCIFI",   // آیتم 2: علمی تخیلی
        "HORROR",  // آیتم 3: ترسناک
        "CLASSIC", // آیتم 4: کلاسیک
        "ART",     // آیتم 5: هنری
        "COMEDY",  // آیتم 6: طنز (تانس)
        "HISTORY"  // آیتم 7: تاریخی
    };

    QStringList selectedGenres;
    for (int i = 0; i < 8; ++i) {
        if (ui->listWidget_genre->item(i)->checkState() == Qt::Checked) {
            selectedGenres.append(englishgenre[i]);
        }
    }

    // چسباندن کلمات با ویرگول
    QString genresString = selectedGenres.join(",");

    QString Message = QString("SIGNUP_NORMALUSER||%1||%2||%3||%4||%5")
                          .arg(name, username, email, password, genresString);
    qDebug() << Message;

    //client->sendMessage(Message);

    // ======== کدهای تست ثبت‌نام کاربر ========
    // تست حالت موفقیت‌آمیز:
    processServerResponse("SIGNUP_NORMALUSER||SUCCESS");

    // تست حالت خطای دوتایی:
    // processServerResponse("SIGNUP_NORMALUSER||FAIL||USERNAME_EXISTS,EMAIL_EXISTS");

    // تست حالت خطای تکی:
    // processServerResponse("SIGNUP_NORMALUSER||FAIL||EMAIL_EXISTS");
}

void Usersignupwindow::on_listWidget_genre_itemChanged(QListWidgetItem *item)
{
    //خاموش کردن موقت سیگنال ها برای جلوگیری از حلقه بی نهایت
    ui->listWidget_genre->blockSignals(true);
    int checkedCount = 0;
    for (int i = 0; i < ui->listWidget_genre->count(); ++i) {
        if (ui->listWidget_genre->item(i)->checkState() == Qt::Checked) {
            checkedCount++;
        }
    }

    // بررسی محدودیت ۳ تایی
    if (checkedCount >= 3) {
        for (int i = 0; i < ui->listWidget_genre->count(); ++i) {
            QListWidgetItem *currentItem = ui->listWidget_genre->item(i);
            if (currentItem->checkState() != Qt::Checked) {
                // این دستور آیتم را خاموش (خاکستری و غیرقابل کلیک) می‌کند
                currentItem->setFlags(currentItem->flags() & ~Qt::ItemIsEnabled);
            }
        }
    } else {
        // اگر کاربر تیکِ یکی را برداشت و تعداد کمتر از ۳ شد، دوباره همه را فعال کن
        for (int i = 0; i < ui->listWidget_genre->count(); ++i) {
            QListWidgetItem *currentItem = ui->listWidget_genre->item(i);

            currentItem->setFlags(currentItem->flags() | Qt::ItemIsEnabled);
        }
    }
    //روشن کردن مجدد سیگنال ها
    ui->listWidget_genre->blockSignals(false);
}

void Usersignupwindow::processServerResponse(const QString &response)
{
    QStringList parts = response.split("||");
    if (parts.isEmpty())
        return;

    if (parts[0] == "SIGNUP_NORMALUSER") {
        if (parts.size() >= 2 && parts[1] == "SUCCESS") {
            UserDashboard *dash = new UserDashboard();
            dash->show();
            emit signupsuccessful();
            this->close();
        } else if (parts.size() >= 3 && parts[1] == "FAIL") {
            // جدا کردن خطاها با ویرگول
            QStringList errors = parts[2].split(",");
            for (int i = 0; i < errors.size(); ++i) {
                if (errors[i] == "USERNAME_EXISTS") {
                    ui->label_UsernameError->setText("این نام کاربری قبلاً ثبت شده است.");
                } else if (errors[i] == "EMAIL_EXISTS") {
                    ui->label_EmailError->setText("این ایمیل قبلاً ثبت شده است.");
                }
            }
        }
    }
}

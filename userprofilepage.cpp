#include "userprofilepage.h"
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QStringList>
#include <QVBoxLayout>
#include "historycardwidget.h"
#include "resetpassworddialog.h"
#include "ui_userprofilepage.h"

Userprofilepage::Userprofilepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Userprofilepage)
{
    ui->setupUi(this);
    ui->label_errorname->setStyleSheet("color: red;");
    ui->label_errorEmail->setStyleSheet("color: red;");
    ui->label_errorname->clear();
    ui->label_errorEmail->clear();

    genreMap["عاشقانه"] = "ROMANCE";
    genreMap["جنایی"] = "CRIME";
    genreMap["علمی تخیلی"] = "SCIFI";
    genreMap["ترسناک"] = "HORROR";
    genreMap["کلاسیک"] = "CLASSIC";
    genreMap["هنری"] = "ART";
    genreMap["طنز"] = "COMEDY";
    genreMap["تاریخی"] = "HISTORY";

    //تست
    //client->sendMessage("GET_DATA_USERPROFILE");
    //دزیافت اطلاعات سرور
    QString dataString = "DATA_USERPROFILE||ALI||ALI1234||MOZ@GMA.CO||ROMANCE,SCIFI||1000||"
                         "3||nbook,nauthor,price,date##nbook,nauthor,price,date";
    information(dataString);
}

Userprofilepage::~Userprofilepage()
{
    delete ui;
}

void Userprofilepage::on_pushButton_topUpbalance_clicked()
{
    QDialog *popUp = new QDialog(this);
    popUp->setWindowTitle("افزایش اعتبار");
    popUp->setFixedSize(300, 150);

    QVBoxLayout *mainLayout = new QVBoxLayout(popUp);

    QLabel *labelInfo = new QLabel("مبلغ مورد نظر را وارد کنید:", popUp);
    mainLayout->addWidget(labelInfo);

    // ساخت اسپین‌باکس با واحد تومان
    QSpinBox *amountSpinBox = new QSpinBox(popUp);
    amountSpinBox->setRange(1000, 5000000); // حداقل هزار تومان، حداکثر ۵ میلیون تومان
    amountSpinBox->setSingleStep(5000);     // با هر کلیک ۵ هزار تومان کم و زیاد شود
    amountSpinBox->setSuffix(" تومان");
    mainLayout->addWidget(amountSpinBox);

    // دکمه‌های تایید و انصراف
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnConfirm = new QPushButton("تایید", popUp);
    QPushButton *btnCancel = new QPushButton("انصراف", popUp);

    btnConfirm->setStyleSheet(
        "background-color: #2ecc71; color: white; font-weight: bold; padding: 6px;");
    btnCancel->setStyleSheet(
        "background-color: #e74c3c; color: white; font-weight: bold; padding: 6px;");

    buttonLayout->addWidget(btnConfirm);
    buttonLayout->addWidget(btnCancel);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, popUp, &QDialog::reject);

    connect(btnConfirm, &QPushButton::clicked, this, [=]() {
        int addAmount = amountSpinBox->value();

        QString currentText = ui->label_balance->text();
        currentText.replace(" تومان", "");        // پاک کردن متن اضافه
        int currentBalance = currentText.toInt(); // تبدیل به عدد صحیح

        int newBalance = currentBalance + addAmount;

        ui->label_balance->setText(QString::number(newBalance) + " تومان");

        QString message = QString("UPDATE_BALANCE||%1").arg(newBalance);
        qDebug() << message;
        // client->sendMessage(message);

        popUp->accept();
    });

    popUp->exec();
}

void Userprofilepage::on_pushButton_editpassword_clicked()
{
    ResetPasswordDialog dialog(this);
    dialog.setupForProfile();

    dialog.exec();
}

void Userprofilepage::on_listWidget_itemChanged(QListWidgetItem *item)
{
    ui->listWidget->blockSignals(true);

    int checkedCount = 0;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked) {
            checkedCount++;
        }
    }

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *currentItem = ui->listWidget->item(i);

        if (checkedCount >= 3 && currentItem->checkState() == Qt::Unchecked) {
            // تیک نخورده ها را غیرفعال می کنیم
            currentItem->setFlags(currentItem->flags() & ~Qt::ItemIsEnabled);
        } else if (checkedCount <= 1 && currentItem->checkState() == Qt::Checked) {
            // تیک خورده را غیر فعال می کنیم
            currentItem->setFlags(currentItem->flags() & ~Qt::ItemIsEnabled);
        } else {
            currentItem->setFlags(currentItem->flags() | Qt::ItemIsEnabled);
        }
    }

    ui->listWidget->blockSignals(false);
}

void Userprofilepage::on_pushButton_sabt_clicked()
{
    // پاک کردن ارورها
    ui->label_errorname->clear();
    ui->label_errorEmail->clear();
    ui->label_errorname->setStyleSheet("color: red;");
    ui->label_errorEmail->setStyleSheet("color: red;");

    QString currentName = ui->lineEdit_name->text().trimmed();
    QString currentEmail = ui->lineEdit_Email->text().trimmed();

    bool hasError = false;

    // بررسی خالی نبودن
    if (currentName.isEmpty()) {
        ui->label_errorname->setText(
            "نام و نام خانوادگی نمی‌تواند خالی باشد.");
        hasError = true;
    }
    if (currentEmail.isEmpty()) {
        ui->label_errorEmail->setText("ایمیل نمی‌تواند خالی باشد.");
        hasError = true;
    } else {
        QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
        if (!emailRegex.match(currentEmail).hasMatch()) {
            ui->label_errorEmail->setText("فرمت ایمیل نامعتبر است.");
            hasError = true;
        }
    }

    if (hasError) {
        return; // توقف در صورت وجود ارور
    }

    // گرفتن ژانرهایی که الان انتخاب شده‌اند
    QStringList currentSelectedGenres;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        if (item && item->checkState() == Qt::Checked) {
            QString faText = item->text().trimmed();
            QString enText = genreMap.value(faText);
            if (!enText.isEmpty()) {
                currentSelectedGenres.append(enText);
            }
        }
    }

    currentSelectedGenres.sort();
    QString currentGenresString = currentSelectedGenres.join(",");

    // بررسی اینکه آیا تغییری ایجاد شده است یا خیر؟
    if (currentName == originalName && currentEmail == originalEmail
        && currentGenresString == originalGenres) {
        // هیچ کدام تغییر نکرده‌اند؛ پس چیزی به سرور ارسال نمی‌کنیم
        return;
    }

    // QString Message = QString("SIGNUP_NORMALU||%1||%2||%3||%4||%5")
    //                       .arg(name, username, email, password, genresString);
    // qDebug() << Message;

    //client->sendMessage(Message);
    // ===================
    //جواب سرور
    //====================
    originalName = currentName;
    originalEmail = currentEmail;
    originalGenres = currentGenresString;

    //بعد از موفق بودن جواب سرور داده ها را ذخیره می کنیم برای انصراف

    // اگر به اینجا رسیدیم یعنی حداقل یکی از موارد تغییر کرده است.
    // کدهای ارسال اطلاعات به سرور را در اینجا قرار دهید:
    // ...
}

void Userprofilepage::on_pushButton_enseraf_clicked()
{
    ui->label_errorname->clear();
    ui->label_errorEmail->clear();

    // بازگردانی همه چیز به حالت اول با استفاده از رشته اولیه
    // if (!originalDataString.isEmpty()) {
    //     information(originalDataString);
    // }
    ui->lineEdit_name->setText(originalName);
    ui->lineEdit_Email->setText(originalEmail);

    QStringList englishGenresList = originalGenres.split(",", Qt::SkipEmptyParts);

    for (const QString &englishGenre : englishGenresList) {
        QString cleanEnglishGenre = englishGenre.trimmed();

        QString persianGenre = genreMap.key(cleanEnglishGenre);

        if (!persianGenre.isEmpty()) {
            // ۴. پیدا کردن آیتم در لیست ویجت (فرض می‌کنیم اسم لیست ویجت شما listWidget_genres است)
            // Qt::MatchExactly باعث می‌شود دقیقا همان متن را پیدا کند
            QList<QListWidgetItem *> foundItems = ui->listWidget->findItems(persianGenre,
                                                                            Qt::MatchExactly);

            // ۵. تیک زدن چک‌باکس آیتم‌های پیدا شده
            for (QListWidgetItem *item : foundItems) {
                item->setCheckState(Qt::Checked);
            }
        }
    }
}

void Userprofilepage::information(QString dataString)
{
    QString userDataString = dataString;
    QStringList parts = dataString.split("||");
    if (parts.size() >= 8) {
        originalName = parts[1].trimmed();
        originalEmail = parts[3].trimmed();

        ui->lineEdit_name->setText(originalName);
        ui->lineEdit_username->setText(parts[2].trimmed());
        ui->label_balance->setText(parts[5].trimmed());
        ui->lineEdit_Email->setText(originalEmail);

        QStringList serverGenresList = parts[4].trimmed().split(",");
        for (QString &g : serverGenresList)
            g = g.trimmed();
        serverGenresList.sort();
        originalGenres = serverGenresList.join(",");

        int checkedCount = 0;

        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

            QString faText = item->text().trimmed();
            QString enText = genreMap.value(faText);

            if (serverGenresList.contains(enText)) {
                item->setCheckState(Qt::Checked);
                checkedCount++;
            } else {
                item->setCheckState(Qt::Unchecked);
            }
        }

        for (int i = 0; i < ui->listWidget->count(); ++i) {
            QListWidgetItem *item = ui->listWidget->item(i);
            if (checkedCount == 1 && item->checkState() == Qt::Checked) {
                item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            } else if (checkedCount == 3 && item->checkState() == Qt::Unchecked) {
                item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            }
        }
    }

    QStringList mainParts = userDataString.split("||");

    if (mainParts.size() <= 7) {
        return;
    }

    int numberOfBooks = mainParts[6].trimmed().toInt();

    ui->label_numberbook->setText(QString::number(numberOfBooks));

    if (numberOfBooks == 0) {
        ui->stackedWidget->setCurrentWidget(ui->page_khali);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_2);

        QLayoutItem *child;
        while ((child = ui->verticalLayout_3->takeAt(0)) != nullptr) {
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }

        if (mainParts.size() >= 8) {
            QString booksString = mainParts[7].trimmed();
            QStringList booksList = booksString.split("##", Qt::SkipEmptyParts);

            for (int i = 0; i < booksList.size(); ++i) {
                // جدا کردن فیلدهای داخل هر کتاب با "کاما" (,)
                QStringList bookFields = booksList[i].split(",");

                if (bookFields.size() >= 4) {
                    QString bName = bookFields[0].trimmed();
                    QString aName = bookFields[1].trimmed();
                    QString price = bookFields[2].trimmed();
                    QString date = bookFields[3].trimmed();

                    // ساخت کارت و مقداردهی
                    HistoryCardwidget *card = new HistoryCardwidget(this);
                    card->setBookData(bName, aName, price, date);

                    ui->verticalLayout_3->insertWidget(0, card);
                }
            }
        }
    }
}
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

    ui->verticalLayout_3->setAlignment(Qt::AlignTop);

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
    //دریافت اطلاعات سرور (رشته تستی اصلاح شده با ۳ کتاب کامل)
    QString dataString = "DATA_USERPROFILE||ALI||ALI1234||MOZ@GMA.CO||ROMANCE,SCIFI||1000||"
                         "3||"
                         "کتاب اول,نویسنده اول,150000,1402/05/12##"
                         "کتاب دوم,نویسنده دوم,90000,1402/06/15##"
                         "کتاب سوم,نویسنده سوم,120000,1402/07/20";
    information(dataString);
}

Userprofilepage::~Userprofilepage()
{
    delete ui;
}

void Userprofilepage::on_pushButton_topUpbalance_clicked()
{
    QDialog popUp(this);
    popUp.setWindowTitle("افزایش اعتبار");
    popUp.setFixedSize(300, 150);

    QVBoxLayout *mainLayout = new QVBoxLayout(&popUp);

    QLabel *labelInfo = new QLabel("مبلغ مورد نظر را وارد کنید:", &popUp);
    mainLayout->addWidget(labelInfo);

    QSpinBox *amountSpinBox = new QSpinBox(&popUp);
    amountSpinBox->setRange(1000, 5000000);
    amountSpinBox->setSingleStep(5000);
    amountSpinBox->setSuffix(" تومان");
    mainLayout->addWidget(amountSpinBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnConfirm = new QPushButton("تایید", &popUp);
    QPushButton *btnCancel = new QPushButton("انصراف", &popUp);

    btnConfirm->setStyleSheet(
        "background-color: #2ecc71; color: white; font-weight: bold; padding: 6px;");
    btnCancel->setStyleSheet(
        "background-color: #e74c3c; color: white; font-weight: bold; padding: 6px;");

    buttonLayout->addWidget(btnConfirm);
    buttonLayout->addWidget(btnCancel);
    mainLayout->addLayout(buttonLayout);

    connect(btnCancel, &QPushButton::clicked, &popUp, &QDialog::reject);

    connect(btnConfirm, &QPushButton::clicked, this, [&]() {
        int addAmount = amountSpinBox->value();

        QString currentText = ui->label_balance->text();
        currentText.replace(" تومان", "");
        int currentBalance = currentText.toInt();

        int newBalance = currentBalance + addAmount;

        ui->label_balance->setText(QString::number(newBalance) + " تومان");

        QString message = QString("UPDATE_BALANCE||%1").arg(newBalance);
        qDebug() << message;

        // client->sendMessage(message);

        popUp.accept();
    });

    popUp.exec();
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
            currentItem->setFlags(currentItem->flags() & ~Qt::ItemIsEnabled);
        } else if (checkedCount <= 1 && currentItem->checkState() == Qt::Checked) {
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
        return;
    }

    // QString Message = QString("UPDATE_PROFILE||%1||%2||%3")
    //                       .arg(currentName, currentEmail, currentGenresString);
    // qDebug() << Message;

    // client->sendMessage(Message);

    // ===================
    // جواب سرور
    // ====================
    originalName = currentName;
    originalEmail = currentEmail;
    originalGenres = currentGenresString;
}

void Userprofilepage::on_pushButton_enseraf_clicked()
{
    ui->label_errorname->clear();
    ui->label_errorEmail->clear();

    ui->lineEdit_name->setText(originalName);
    ui->lineEdit_Email->setText(originalEmail);

    ui->listWidget->blockSignals(true);

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *item = ui->listWidget->item(i);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setCheckState(Qt::Unchecked);
    }

    QStringList englishGenresList = originalGenres.split(",", Qt::SkipEmptyParts);

    for (const QString &englishGenre : englishGenresList) {
        QString cleanEnglishGenre = englishGenre.trimmed();
        QString persianGenre = genreMap.key(cleanEnglishGenre);

        if (!persianGenre.isEmpty()) {
            QList<QListWidgetItem *> foundItems = ui->listWidget->findItems(persianGenre,
                                                                            Qt::MatchExactly);

            for (QListWidgetItem *item : foundItems) {
                item->setCheckState(Qt::Checked);
            }
        }
    }

    ui->listWidget->blockSignals(false);
    on_listWidget_itemChanged(nullptr);
}

void Userprofilepage::information(QString dataString)
{
    QStringList mainParts = dataString.split("||");

    if (mainParts.size() < 8) {
        return;
    }

    originalName = mainParts[1].trimmed();
    originalEmail = mainParts[3].trimmed();

    ui->lineEdit_name->setText(originalName);
    ui->lineEdit_username->setText(mainParts[2].trimmed());
    ui->lineEdit_Email->setText(originalEmail);
    ui->label_balance->setText(mainParts[5].trimmed() + " تومان");

    QStringList serverGenresList = mainParts[4].trimmed().split(",");
    for (QString &g : serverGenresList)
        g = g.trimmed();
    serverGenresList.sort();
    originalGenres = serverGenresList.join(",");

    int checkedCount = 0;
    ui->listWidget->blockSignals(true);
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
    ui->listWidget->blockSignals(false);
    on_listWidget_itemChanged(nullptr);

    int numberOfBooks = mainParts[6].trimmed().toInt();
    ui->label_numberbook->setText(QString::number(numberOfBooks));

    if (numberOfBooks == 0) {
        ui->stackedWidget->setCurrentWidget(ui->page_khali);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_2);
        while (ui->verticalLayout_3->count() > 1) {
            QLayoutItem *child = ui->verticalLayout_3->takeAt(0);
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }

        QString booksString = mainParts[7].trimmed();
        QStringList booksList = booksString.split("##", Qt::SkipEmptyParts);

        for (int i = 0; i < booksList.size(); ++i) {
            QStringList bookFields = booksList[i].split(",");

            if (bookFields.size() >= 4) {
                QString bName = bookFields[0].trimmed();
                QString aName = bookFields[1].trimmed();
                QString price = bookFields[2].trimmed();
                QString date = bookFields[3].trimmed();

                HistoryCardwidget *card = new HistoryCardwidget(this);
                card->setBookData(bName, aName, price, date);

                ui->verticalLayout_3->insertWidget(ui->verticalLayout_3->count() - 1,
                                                   card,
                                                   0,
                                                   Qt::AlignTop);
            }
        }
    }
}
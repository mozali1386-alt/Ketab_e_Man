#include "storepage.h"
#include <QDebug>
#include <QMap>
#include <QStringList>
#include "bookdetails.h"
#include "storebookwidget.h"
#include "ui_storepage.h"

Storepage::Storepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Storepage)
{
    ui->setupUi(this);
}

Storepage::~Storepage()
{
    delete ui;
}

void Storepage::on_pushButton_search_clicked()
{
    QString bookName = ui->lineEdit_bookname->text().trimmed();
    QString author = ui->lineEdit_authorname->text().trimmed();
    QString publisher = ui->lineEdit_publishername->text().trimmed();

    QString selectedGenreFarsi = ui->comboBox_genre->currentText().trimmed();
    QString selectedDisplayFarsi = ui->comboBox_displaytype->currentText().trimmed();

    QString sendBook = bookName.isEmpty() ? "EMPTY" : bookName;
    QString sendAuthor = author.isEmpty() ? "EMPTY" : author;
    QString sendPublisher = publisher.isEmpty() ? "EMPTY" : publisher;

    QString sendGenre = "ALL";
    if (selectedGenreFarsi == "عاشقانه")
        sendGenre = "ROMANCE";
    else if (selectedGenreFarsi == "علمی تخیلی")
        sendGenre = "SCIFI";
    else if (selectedGenreFarsi == "ترسناک")
        sendGenre = "HORROR";
    else if (selectedGenreFarsi == "کلاسیک")
        sendGenre = "CLASSIC";
    else if (selectedGenreFarsi == "جنایی")
        sendGenre = "CRIME";
    else if (selectedGenreFarsi == "هنری")
        sendGenre = "ART";
    else if (selectedGenreFarsi == "طنز")
        sendGenre = "COMEDY";
    else if (selectedGenreFarsi == "تاریخی")
        sendGenre = "HISTORY";

    QString sendDisplay = "ALL";
    if (selectedDisplayFarsi == "پیشنهادی ها")
        sendDisplay = "RECOMMENDED";
    else if (selectedDisplayFarsi == "محبوب ها")
        sendDisplay = "POPULAR";
    else if (selectedDisplayFarsi == "جدید ها")
        sendDisplay = "NEW";
    else if (selectedDisplayFarsi == "پرفروش ها")
        sendDisplay = "BESTSELLER";
    else if (selectedDisplayFarsi == "رایگان ها")
        sendDisplay = "FREE";

    QString message = QString("SEARCH_STOREPAGE||%1||%2||%3||%4||%5")
                          .arg(sendBook, sendAuthor, sendPublisher, sendGenre, sendDisplay);

    if (lastsearchdisplay == message)
        return;

    lastsearchdisplay = message;

    // ارسال درخواست جستجو به سرور
    // client->sendMessage(message);
    // ================== شروع تست (بعداً پاک کن) ==================
    processServerResponse("SEARCH_RESULT||1,2,3");
    // =========================================================
}

// تابع درخواست اطلاعات کامل یک کتاب با استفاده از آیدی
void Storepage::requestBookSummary(const QString &bookId)
{
    // QString message = "GET_BOOK_SUMMARY||" + bookId;
    // client->sendMessage(message);

    // ================== شروع تست (بعداً پاک کن) ==================
    if (bookId == "1") {
        processServerResponse("BOOK_SUMMARY||1||NO_IMAGE||سمفونی مردگان||عباس معروفی||130000||4.8");
    } else if (bookId == "2") {
        processServerResponse("BOOK_SUMMARY||NOT_FOUND");
    } else if (bookId == "3") {
        processServerResponse(
            "BOOK_SUMMARY||3||NO_IMAGE||شازده کوچولو||آنتوان دو سنت اگزوپری||120000||4.9");
    }
    // =========================================================
}

void Storepage::processServerResponse(const QString &response)
{
    QStringList mainParts = response.split("||");
    if (mainParts.size() < 2)
        return;

    QString command = mainParts[0].trimmed();
    QString fieldTwo = mainParts[1].trimmed();

    if (command == "SEARCH_RESULT") {
        while (ui->verticalLayout_5->count() > 1) {
            QLayoutItem *child = ui->verticalLayout_5->takeAt(0);
            if (child->widget()) {
                delete child->widget();
            }
            delete child;
        }

        // if (fieldTwo == "NOT_FOUND" || fieldTwo.isEmpty()) {
        //     return;
        // }

        if (fieldTwo == "EMPTY" || fieldTwo.isEmpty()) {
            return;
        }
        QStringList ids = fieldTwo.split(",", Qt::SkipEmptyParts);
        for (int i = 0; i < ids.size(); ++i) {
            requestBookSummary(ids[i].trimmed());
        }
    }

    else if (command == "BOOK_SUMMARY") {
        if (fieldTwo == "NOT_FOUND") {
            return; // عبور از این آیدی بدون ساخت کارت
        }

        if (mainParts.size() >= 7) {
            QString bookId = fieldTwo;
            QString imageBase64 = mainParts[2].trimmed();
            QString bookName = mainParts[3].trimmed();
            QString author = mainParts[4].trimmed();
            QString price = mainParts[5].trimmed();
            QString score = mainParts[6].trimmed();

            StorebookWidget *book = new StorebookWidget(this);

            book->setBookId(bookId);
            book->setBookData(imageBase64, bookName, author, price, score);

            connect(book, &StorebookWidget::bookClicked, this, [=](QString clickedId) {
                Bookdetails *detailsWindow = new Bookdetails(nullptr, clickedId);
                detailsWindow->setAttribute(Qt::WA_DeleteOnClose);
                detailsWindow->show();
            });

            ui->verticalLayout_5->insertWidget(ui->verticalLayout_5->count() - 1, book);
        }
    }
}
void Storepage::refreshStore()
{
    // پاک کردن حافظه جستجوی قبلی تا اجازه درخواست مجدد داده شود
    lastsearchdisplay.clear();
    on_pushButton_search_clicked();
}
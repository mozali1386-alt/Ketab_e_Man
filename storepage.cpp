#include "storepage.h"
#include <QMap>
#include <QStringList>
#include "storebookwidget.h"
#include "ui_storepage.h"

Storepage::Storepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Storepage)
{
    ui->setupUi(this);

    QString testServerData = "STORE_BOOKS||"
                             "book1.jpg,سمفونی مردگان,عباس معروفی,130000,4.8##"
                             "book2.jpg,بوف کور,صادق هدایت,90000,4.5##"
                             "book3.jpg,شازده کوچولو,آنتوان دو سنت اگزوپری,120000,4.9";

    // همان رشته را به تابع پردازش پاس می‌دهیم
    processServerResponse(testServerData);
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

    if (bookName.isEmpty() && author.isEmpty() && publisher.isEmpty()) {
        return; // بازگشت بی‌صدا (بدون هیچ واکنشی)
    }

    QString sendBook = bookName.isEmpty() ? "EMPTY" : bookName;
    QString sendAuthor = author.isEmpty() ? "EMPTY" : author;
    QString sendPublisher = publisher.isEmpty() ? "EMPTY" : publisher;

    QString message = QString("SEARCH_STOREPAGE||%1||%2||%3")
                          .arg(sendBook, sendAuthor, sendPublisher);

    if (lastsearchdisplay == message)
        return;

    lastsearchdisplay = message; //ذخیره جستجوی جدید
    qDebug() << message;

    // client->sendMessage(message);
}

void Storepage::on_pushButton_display_clicked()
{
    QString selectedGenreFarsi = ui->comboBox_genre->currentText().trimmed();
    QString selectedDisplayFarsi = ui->comboBox_displaytype->currentText().trimmed();

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

    QString message = QString("DISPLAY_STOREPAGE||%1||%2").arg(sendGenre, sendDisplay);

    if (lastsearchdisplay == message)
        return;

    lastsearchdisplay = message;
    qDebug() << "Sending to server: " << message;

    // client->sendMessage(message);
}

void Storepage::processServerResponse(const QString &response)
{
    while (ui->verticalLayout_5->count() > 1) {
        QLayoutItem *child = ui->verticalLayout_5->takeAt(0);
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    QStringList mainParts = response.split("||");
    if (mainParts.size() < 2)
        return;

    QString data = mainParts[1];
    if (data.isEmpty())
        return;

    QStringList booksList = data.split("##");
    for (int i = 0; i < booksList.size(); ++i) {
        QStringList fields = booksList[i].split(",");

        if (fields.size() == 5) {
            StorebookWidget *book = new StorebookWidget(this);
            book->setBookData(fields[0], fields[1], fields[2], fields[3], fields[4]);

            ui->verticalLayout_5->insertWidget(ui->verticalLayout_5->count() - 1, book);
        }
    }
}

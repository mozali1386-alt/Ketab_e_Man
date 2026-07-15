#include "storepage.h"
#include <QMap>
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
    QMap<QString, QString> genreMap;
    genreMap["همه ژانرها"] = "ALL";
    genreMap["عاشقانه"] = "ROMANCE";
    genreMap["علمی تخیلی"] = "SCIFI";
    genreMap["ترسناک"] = "HORROR";
    genreMap["کلاسیک"] = "CLASSIC";
    genreMap["جنایی"] = "ART";
    genreMap["هنری"] = "COMEDY";
    genreMap["طنز"] = "CLASSIC";
    genreMap["تاریخی"] = "HISTORY";

    QMap<QString, QString> displayMap;
    displayMap["همه کتاب ها"] = "ALL";
    displayMap["پیشنهادی ها"] = "RECOMMENDED";
    displayMap["محبوب ها"] = "POPULAR";
    displayMap["جدید ها"] = "NEW";
    displayMap["پرفروش ها"] = "BESTSELLER";
    displayMap["رایگان ها"] = "FREE";

    QString selectedGenreFarsi = ui->comboBox_genre->currentText().trimmed();
    QString selectedDisplayFarsi = ui->comboBox_displaytype->currentText().trimmed();

    QString sendGenre = genreMap.value(selectedGenreFarsi, "ALL");
    QString sendDisplay = displayMap.value(selectedDisplayFarsi, "ALL");

    QString message = QString("DISPLAY_STOREPAGE||%1||%2").arg(sendGenre, sendDisplay);

    if (lastsearchdisplay == message)
        return;

    lastsearchdisplay = message;
    qDebug() << message;

    //client->sendMessage(message);
}

#include "historycardwidget.h"
#include "ui_historycardwidget.h"

HistoryCardwidget::HistoryCardwidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HistoryCardwidget)
{
    ui->setupUi(this);
}

HistoryCardwidget::~HistoryCardwidget()
{
    delete ui;
}
void HistoryCardwidget::setBookData(QString bookName,
                                    QString authorName,
                                    QString price,
                                    QString date)
{
    ui->label_bookname->setText(bookName);
    ui->label_anothername->setText(authorName);
    ui->label_price->setText(price + " تومان");
    ui->label_tarikh->setText(date);
}

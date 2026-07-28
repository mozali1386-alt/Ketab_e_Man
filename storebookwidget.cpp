#include "storebookwidget.h"
#include <QByteArray>
#include <QMouseEvent>
#include <QPixmap>
#include "ui_storebookwidget.h"

StorebookWidget::StorebookWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StorebookWidget)
{
    ui->setupUi(this);
}

StorebookWidget::~StorebookWidget()
{
    delete ui;
}

void StorebookWidget::setBookData(const QString &image,
                                  const QString &title,
                                  const QString &author,
                                  const QString &finalPrice,
                                  const QString &score)
{
    if (image == "NO_IMAGE" || image.isEmpty()) {
        ui->label_pikcher->setText("بدون تصویر");
    } else {
        QByteArray byteArray = QByteArray::fromBase64(image.toUtf8());
        QPixmap pixmap;
        if (pixmap.loadFromData(byteArray)) {
            ui->label_pikcher->setPixmap(pixmap);
            ui->label_pikcher->setScaledContents(true);
        } else {
            ui->label_pikcher->setText("خطا در عکس");
        }
    }
    ui->label_bookname->setText(title);
    ui->label_anuthername->setText(author);
    ui->label_price->setText(finalPrice + " تومان");
    ui->label_score->setText(score);
}

void StorebookWidget::setBookId(const QString &id)
{
    currentBookId = id;
}

void StorebookWidget::mousePressEvent(QMouseEvent *event)
{
    emit bookClicked(currentBookId);
    QWidget::mousePressEvent(event);
}
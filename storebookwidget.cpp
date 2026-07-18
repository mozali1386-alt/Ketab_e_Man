#include "storebookwidget.h"
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
    ui->label_pikcher->setText(image);
    ui->label_bookname->setText(title);
    ui->label_anuthername->setText(author);
    ui->label_price->setText(finalPrice + " تومان");
    ui->label_score->setText(score);
}
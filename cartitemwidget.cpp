#include "cartitemwidget.h"
#include "ui_cartitemwidget.h"

CartitemWidget::CartitemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CartitemWidget)
{
    ui->setupUi(this);

    // مقداردهی اولیه و ایمن متغیرها
    mainPriceValue = 0;
    offPriceValue = 0;
    finalPriceValue = 0;
}

CartitemWidget::~CartitemWidget()
{
    delete ui;
}

void CartitemWidget::setBookData(
    const QString &image, const QString &title, const QString &author, int mainPrice, int discount)
{
    mainPriceValue = mainPrice;
    offPriceValue = discount;
    finalPriceValue = mainPrice - discount;

    ui->label_pikcherbook->setText(image);
    ui->label_bookname->setText(title);
    ui->label_authername->setText(author);
    ui->label_pricenahaii->setText(QString::number(finalPriceValue) + " تومان");

    // اگر تخفیف نداشت، لیبل‌های تخفیف و قیمت اصلی را مخفی کن
    if (discount == 0) {
        ui->label_priceasli->hide();
        ui->label_discount->hide();
    } else {
        ui->label_priceasli->show();
        ui->label_discount->show();
        ui->label_priceasli->setText(QString::number(mainPrice) + " تومان");
        ui->label_discount->setText(QString::number(discount) + " تومان تخفیف");
    }
}

bool CartitemWidget::isChecked()
{
    return ui->checkBox_choose->isChecked();
}

void CartitemWidget::setChecked(bool state)
{
    ui->checkBox_choose->setChecked(state);
}

int CartitemWidget::getMainPrice()
{
    return mainPriceValue;
}

int CartitemWidget::getOffPrice()
{
    return offPriceValue;
}

int CartitemWidget::getFinalPrice()
{
    return finalPriceValue;
}

void CartitemWidget::on_toolButton_trash_clicked()
{
    emit itemDeleted(this);
}

void CartitemWidget::on_checkBox_choose_clicked()
{
    emit itemToggled();
}
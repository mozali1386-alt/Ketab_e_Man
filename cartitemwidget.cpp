#include "cartitemwidget.h"
#include <QByteArray>
#include <QPixmap>
#include "ui_cartitemwidget.h"

CartitemWidget::CartitemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CartitemWidget)
{
    ui->setupUi(this);

    mainPriceValue = 0;
    offPriceValue = 0;
    finalPriceValue = 0;
}

CartitemWidget::~CartitemWidget()
{
    delete ui;
}

void CartitemWidget::setBookData(const QString &id,
                                 const QString &image,
                                 const QString &title,
                                 const QString &author,
                                 int mainPrice,
                                 int discount)
{
    currentBookId = id;
    mainPriceValue = mainPrice;
    offPriceValue = discount;
    finalPriceValue = mainPrice - discount;

    // تبدیل رشته Base64 به عکس واقعی
    if (image == "NO_IMAGE" || image.isEmpty()) {
        ui->label_pikcherbook->setText("بدون تصویر");
    } else {
        QByteArray byteArray = QByteArray::fromBase64(image.toUtf8());
        QPixmap pixmap;
        if (pixmap.loadFromData(byteArray)) {
            ui->label_pikcherbook->setPixmap(pixmap);
            ui->label_pikcherbook->setScaledContents(true);
        } else {
            ui->label_pikcherbook->setText("خطا در عکس");
        }
    }

    ui->label_bookname->setText(title);
    ui->label_authername->setText(author);
    ui->label_pricenahaii->setText(QString::number(finalPriceValue) + " تومان");

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

QString CartitemWidget::getBookId() const
{
    return currentBookId;
}
int CartitemWidget::getMainPrice() const
{
    return mainPriceValue;
}
int CartitemWidget::getOffPrice() const
{
    return offPriceValue;
}
int CartitemWidget::getFinalPrice() const
{
    return finalPriceValue;
}

void CartitemWidget::on_toolButton_trash_clicked()
{
    emit itemDeleted(this);
}
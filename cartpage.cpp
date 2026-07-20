#include "cartpage.h"
#include <QList>
#include <QMessageBox>
#include <QStringList>
#include "ui_cartpage.h"

Cartpage::Cartpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Cartpage)
{
    ui->setupUi(this);

    requestCartItems();
}

Cartpage::~Cartpage()
{
    delete ui;
}

void Cartpage::requestCartItems()
{
    QString message = "GET_CART_ITEMS";
    // client->sendMessage(message);

    // ================== شروع تست ==================
    processServerResponse("CART_RESULT||1,3");
    // ==============================================
}

void Cartpage::requestCartBookSummary(const QString &bookId)
{
    QString message = "GET_CART_BOOK_SUMMARY||" + bookId;
    // client->sendMessage(message);

    // ================== شروع تست ==================
    if (bookId == "1") {
        processServerResponse(
            "CART_BOOK_SUMMARY||1||NO_IMAGE||سمفونی مردگان||عباس معروفی||150000||20000");
    } else if (bookId == "3") {
        processServerResponse(
            "CART_BOOK_SUMMARY||3||NO_IMAGE||شازده کوچولو||آنتوان دو سنت اگزوپری||120000||0");
    }
    // ==============================================
}

void Cartpage::processServerResponse(const QString &response)
{
    QStringList mainParts = response.split("||");
    if (mainParts.size() < 2)
        return;

    QString command = mainParts[0].trimmed();
    QString fieldTwo = mainParts[1].trimmed();

    if (command == "CART_RESULT") {
        clearCartUI();

        if (fieldTwo == "EMPTY" || fieldTwo.isEmpty()) {
            updateSummary();
            return;
        }

        QStringList ids = fieldTwo.split(",", Qt::SkipEmptyParts);
        for (int i = 0; i < ids.size(); ++i) {
            requestCartBookSummary(ids[i].trimmed());
        }
    }

    else if (command == "CART_BOOK_SUMMARY") {
        if (fieldTwo == "NOT_FOUND" || mainParts.size() < 7)
            return;

        CartitemWidget *newBook = new CartitemWidget(this);
        newBook->setBookData(fieldTwo,
                             mainParts[2].trimmed(),
                             mainParts[3].trimmed(),
                             mainParts[4].trimmed(),
                             mainParts[5].toInt(),
                             mainParts[6].toInt());

        addNewBookToCart(newBook);
    }

    else if (command == "CHECKOUT_RESULT") {
        if (fieldTwo == "SUCCESS") {
            QMessageBox::information(this,
                                     "موفقیت",
                                     "خرید شما با موفقیت انجام شد و کتاب‌ها به "
                                     "کتابخانه "
                                     "شما "
                                     "اضافه شدند.");
            clearCartUI();   // خالی کردن گرافیکی سبد
            updateSummary(); // آپدیت مبالغ به صفر و نمایش صفحه خالی
        } else if (fieldTwo == "FAILED") {
            QMessageBox::warning(this,
                                 "خطا در پرداخت",
                                 "موجودی کیف پول شما کافی نمی‌باشد. لطفاً موجودی خود "
                                 "را "
                                 "افزایش "
                                 "دهید.");
        }
    }
}

void Cartpage::addNewBookToCart(CartitemWidget *newBook)
{
    int insertIndex = ui->verticalLayout_5->count() - 1;
    ui->verticalLayout_5->insertWidget(insertIndex, newBook);

    connect(newBook, &CartitemWidget::itemDeleted, this, &Cartpage::removeBookFromCart);
    updateSummary();
}

void Cartpage::removeBookFromCart(CartitemWidget *bookToRemove)
{
    QString message = "REMOVE_CART||" + bookToRemove->getBookId();
    // client->sendMessage(message);

    ui->verticalLayout_5->removeWidget(bookToRemove);
    bookToRemove->hide();
    bookToRemove->setParent(nullptr);
    bookToRemove->deleteLater();

    updateSummary();
}

// دکمه ثبت و نهایی سازی
void Cartpage::on_pushButton_sabt_clicked()
{
    // ارسال درخواست خرید کل سبد به سرور
    QString message = "CHECKOUT_CART";
    // client->sendMessage(message);

    // ================== شروع تست ==================
    // برای تست موجودی کافی:
    //processServerResponse("CHECKOUT_RESULT||SUCCESS");

    // برای تست موجودی ناکافی، خط بالا را کامنت کن و خط پایین را اجرا کن:
    processServerResponse("CHECKOUT_RESULT||FAILED");
    // =============================================
}

// تابع کمکی برای پاکسازی کامل لیست (جلوگیری از کدهای تکراری)
void Cartpage::clearCartUI()
{
    while (ui->verticalLayout_5->count() > 1) {
        QLayoutItem *child = ui->verticalLayout_5->takeAt(0);
        if (child->widget())
            delete child->widget();
        delete child;
    }
}

void Cartpage::updateSummary()
{
    int checkedItems = 0;
    int sumMainPrice = 0;
    int sumOff = 0;
    int sumFinal = 0;

    QList<CartitemWidget *> items = this->findChildren<CartitemWidget *>();
    int totalItems = items.size();

    // منطق نمایش صفحه خالی یا پر
    if (totalItems == 0) {
        ui->stackedWidget->setCurrentWidget(ui->page_khali);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_asli);
    }

    // جمع زدن تمام آیتم‌ها (چون چک‌باکس حذف شده، همه حساب می‌شوند)
    for (CartitemWidget *item : items) {
        checkedItems++;
        sumMainPrice += item->getMainPrice();
        sumOff += item->getOffPrice();
        sumFinal += item->getFinalPrice();
    }

    ui->label_jensnumber->setText(QString::number(checkedItems));
    ui->label_totalpriceasli->setText(QString::number(sumMainPrice));
    ui->label_discount->setText(QString::number(sumOff));
    ui->label_totalprice_discountnext->setText(QString("مبلغ نهایی: %1 تومان").arg(sumFinal));
}
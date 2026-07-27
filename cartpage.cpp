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
}

Cartpage::~Cartpage()
{
    delete ui;
}

void Cartpage::requestCartItems()
{
    QString message = "GET_CART_ITEMS";
    // client->sendMessage(message);
}

void Cartpage::requestCartBookSummary(const QString &bookId)
{
    QString message = "GET_CART_BOOK_SUMMARY||" + bookId;
    // client->sendMessage(message);
}

void Cartpage::processServerResponse(const QString &response)
{
    QStringList mainParts = response.split("||");
    if (mainParts.size() < 2)
        return;

    QString command = mainParts[0].trimmed();
    QString fieldTwo = mainParts[1].trimmed();

    if (command == "CART_RESULT") {
        clearCartUI(); // ابتدا ظاهر سبد را پاک می‌کنیم

        QString itemCountStr = (mainParts.size() > 1) ? mainParts[1].trimmed() : "0";

        if (itemCountStr == "0" || itemCountStr == "EMPTY") {
            expectedCartItems = 0;
            loadedCartItems = 0;
            isCheckingOut = false; // لغو حالت خرید
            updateSummary();       // نمایش صفحه "سبد خرید خالی است"
            return;
        }
        // گرفتن تعداد کل آیتم‌ها
        expectedCartItems = itemCountStr.toInt();
        loadedCartItems = 0;

        if (mainParts.size() >= 3) {
            QStringList ids = mainParts[2].split(",", Qt::SkipEmptyParts);
            for (int i = 0; i < ids.size(); ++i) {
                requestCartBookSummary(ids[i].trimmed());
            }
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

        // یک کتاب اضافه شد، شمارنده را بالا می‌بریم
        loadedCartItems++;

        // وقتی همه کتاب‌ها کامل از سرور دریافت شدند
        if (loadedCartItems == expectedCartItems) {
            // چک می‌کنیم آیا کاربر دکمه ثبت خرید را زده بود؟
            if (isCheckingOut) {
                // استخراج قیمت جدید پس از آپدیت
                QString newPriceStr = ui->label_totalprice_discountnext->text();
                newPriceStr.remove("مبلغ نهایی: ").remove(" تومان").trimmed();
                int newTotalPrice = newPriceStr.toInt();

                if (newTotalPrice == savedOldPrice) {
                    // قیمت تغییر نکرده! ارسال درخواست کسر از کیف پول به سرور
                    QString message = "CHECKOUT_CART";
                    // client->sendMessage(message);

                } else {
                    // قیمت تغییر کرده! توقف عملیات و نمایش پیام (خرید لغو می‌شود)
                    QMessageBox::warning(this,
                                         "تغییر قیمت",
                                         "قیمت یا موجودی کتاب‌ها تغییر کرده است. سبد خرید "
                                         "شما "
                                         "به‌روزرسانی شد.");
                }

                // در هر دو صورت، حالت خرید تمام می‌شود
                isCheckingOut = false;
            }
        }
    }

    else if (command == "CHECKOUT_RESULT") {
        isCheckingOut = false; // اطمینان از خروج از حالت اعتبارسنجی

        if (fieldTwo == "SUCCESS") {
            QMessageBox::information(this,
                                     "موفقیت",
                                     "خرید شما با موفقیت انجام شد و کتاب‌ها به کتابخانه "
                                     "شما "
                                     "اضافه شدند.");
            clearCartUI();
            updateSummary();
        } else if (fieldTwo == "FAILED") {
            QMessageBox::warning(this,
                                 "خطا در پرداخت",
                                 "موجودی کیف پول شما کافی نمی‌باشد. لطفاً موجودی خود را "
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
    QString currentPriceStr = ui->label_totalprice_discountnext->text();
    currentPriceStr.remove("مبلغ نهایی: ").remove(" تومان").trimmed();
    savedOldPrice = currentPriceStr.toInt();

    // فعال کردن حالت خرید
    isCheckingOut = true;
    requestCartItems();
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
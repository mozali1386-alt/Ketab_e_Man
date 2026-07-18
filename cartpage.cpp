#include "cartpage.h"
#include <QStringList>
#include "ui_cartpage.h"

Cartpage::Cartpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Cartpage)
{
    ui->setupUi(this);
    ui->checkBox_checkall->setTristate(true);

    // شبیه‌سازی دیتای دریافتی از سرور (کتاب دوم بدون تخفیف است)
    QString mockServerData = "CART_ITEMS||"
                             "pic1.png,سمفونی مردگان,عباس معروفی,150000,20000##"
                             "pic2.png,بوف کور,صادق هدایت,90000,0##"
                             "pic3.png,شازده کوچولو,آنتوان دو سنت اگزوپری,120000,15000";

    loadCartItems(mockServerData);
}

Cartpage::~Cartpage()
{
    delete ui;
}

void Cartpage::loadCartItems(const QString &serverResponse)
{
    QStringList mainParts = serverResponse.split("||");
    if (mainParts.size() < 2)
        return;

    QString data = mainParts[1];
    if (data.isEmpty()) {
        updateSummary();
        return;
    }

    QStringList booksList = data.split("##");
    for (int i = 0; i < booksList.size(); ++i) {
        QStringList fields = booksList[i].split(",");

        if (fields.size() == 5) {
            CartitemWidget *newBook = new CartitemWidget(this);
            // تبدیل متن به عدد در لحظه پاس دادن به تابع
            newBook->setBookData(fields[0],
                                 fields[1],
                                 fields[2],
                                 fields[3].toInt(),
                                 fields[4].toInt());
            newBook->setChecked(
                true); // پیش‌فرض کتاب در سبد تیک‌خورده باشد
            addNewBookToCart(newBook);
        }
    }
    updateSummary();
}

void Cartpage::addNewBookToCart(CartitemWidget *newBook)
{
    // استفاده از insertWidget برای قرارگیری بالای اسپیسر
    int insertIndex = ui->verticalLayout_5->count() - 1;
    ui->verticalLayout_5->insertWidget(insertIndex, newBook);

    connect(newBook, &CartitemWidget::itemToggled, this, &Cartpage::updateSummary);
    connect(newBook, &CartitemWidget::itemDeleted, this, &Cartpage::removeBookFromCart);
}

void Cartpage::removeBookFromCart(CartitemWidget *bookToRemove)
{
    ui->verticalLayout_5->removeWidget(bookToRemove);
    bookToRemove->hide();

    // این خط اضافه می‌شود: قطع کامل ارتباط این کتاب با صفحه اصلی
    bookToRemove->setParent(nullptr);

    bookToRemove->deleteLater();

    updateSummary();
}

void Cartpage::on_checkBox_checkall_clicked()
{
    bool selectAll = (ui->checkBox_checkall->checkState() != Qt::Unchecked);

    // پیدا کردن تمام ویجت‌های کتاب در صفحه
    QList<CartitemWidget *> items = this->findChildren<CartitemWidget *>();

    // یک حلقه بسیار کوتاه برای تیک زدن همه کتاب‌ها
    for (CartitemWidget *item : items) {
        item->blockSignals(true);
        item->setChecked(selectAll);
        item->blockSignals(false);
    }

    updateSummary();
}

void Cartpage::updateSummary()
{
    int checkedItems = 0;
    int sumMainPrice = 0;
    int sumOff = 0;
    int sumFinal = 0;

    QList<CartitemWidget *> items = this->findChildren<CartitemWidget *>();
    int totalItems = items.size();

    if (totalItems == 0) {
        ui->stackedWidget->setCurrentWidget(ui->page_khali);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->page_asli);
    }
    for (CartitemWidget *item : items) {
        if (item->isChecked()) {
            checkedItems++;
            sumMainPrice += item->getMainPrice();
            sumOff += item->getOffPrice();
            sumFinal += item->getFinalPrice();
        }
    }

    ui->checkBox_checkall->blockSignals(true);
    if (totalItems == 0 || checkedItems == 0) {
        ui->checkBox_checkall->setCheckState(Qt::Unchecked);
    } else if (checkedItems == totalItems) {
        ui->checkBox_checkall->setCheckState(Qt::Checked);
    } else {
        ui->checkBox_checkall->setCheckState(Qt::PartiallyChecked);
    }
    ui->checkBox_checkall->blockSignals(false);

    ui->label_jensnumber->setText(QString::number(checkedItems));
    ui->label_totalpriceasli->setText(QString::number(sumMainPrice));
    ui->label_discount->setText(QString::number(sumOff));

    ui->label_totalprice_discountnext->setText(QString("مبلغ نهایی: %1 تومان").arg(sumFinal));
}
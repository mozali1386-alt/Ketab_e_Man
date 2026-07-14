#include "userdashboard.h"
#include "ui_userdashboard.h"

UserDashboard::UserDashboard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UserDashboard)
{
    ui->setupUi(this);
    // به استک ویجت می‌گوییم صفحه فروشگاه را نشان بده
    ui->stackedWidget->setCurrentWidget(ui->page_store);

    // به دکمه فروشگاه می‌گوییم به حالت انتخاب‌شده (سبز رنگ) در بیاید
    ui->pushButton_store->setChecked(true);
}

UserDashboard::~UserDashboard()
{
    delete ui;
}

void UserDashboard::on_pushButton_store_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_store);
}

void UserDashboard::on_pushButton_cart_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_cart);
}

void UserDashboard::on_pushButton_mylibrary_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_library);
}

void UserDashboard::on_pushButton_profile_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_profile);
}

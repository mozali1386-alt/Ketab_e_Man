#include "publisherdashboard.h"
#include "ui_publisherdashboard.h"

Publisherdashboard::Publisherdashboard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Publisherdashboard)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_managementbook);

    ui->pushButton_managmentbook->setChecked(true);
}

Publisherdashboard::~Publisherdashboard()
{
    delete ui;
}

void Publisherdashboard::on_pushButton_managmentbook_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_managementbook);
}

void Publisherdashboard::on_pushButton_statistic_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_statistic);
}

void Publisherdashboard::on_pushButton_hesabkarbar_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_profile);
}

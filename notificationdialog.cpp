#include "notificationdialog.h"
#include "ui_notificationdialog.h"

Notificationdialog::Notificationdialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Notificationdialog)
{
    ui->setupUi(this);
}

Notificationdialog::~Notificationdialog()
{
    delete ui;
}

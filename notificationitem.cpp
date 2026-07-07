#include "notificationitem.h"
#include "ui_notificationitem.h"

Notificationitem::Notificationitem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Notificationitem)
{
    ui->setupUi(this);
}

Notificationitem::~Notificationitem()
{
    delete ui;
}

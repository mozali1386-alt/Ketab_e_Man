#include "userprofilepage.h"
#include "ui_userprofilepage.h"

Userprofilepage::Userprofilepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Userprofilepage)
{
    ui->setupUi(this);
}

Userprofilepage::~Userprofilepage()
{
    delete ui;
}

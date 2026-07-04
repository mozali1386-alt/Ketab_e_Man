#include "cartpage.h"
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

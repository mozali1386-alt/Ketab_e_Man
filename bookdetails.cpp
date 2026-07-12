#include "bookdetails.h"
#include "ui_bookdetails.h"

Bookdetails::Bookdetails(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Bookdetails)
{
    ui->setupUi(this);
}

Bookdetails::~Bookdetails()
{
    delete ui;
}

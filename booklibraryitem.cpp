#include "booklibraryitem.h"
#include "ui_booklibraryitem.h"

Booklibraryitem::Booklibraryitem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Booklibraryitem)
{
    ui->setupUi(this);
}

Booklibraryitem::~Booklibraryitem()
{
    delete ui;
}

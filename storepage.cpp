#include "storepage.h"
#include "ui_storepage.h"

Storepage::Storepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Storepage)
{
    ui->setupUi(this);
}

Storepage::~Storepage()
{
    delete ui;
}

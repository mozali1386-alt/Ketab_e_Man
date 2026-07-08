#include "publisherprofilepage.h"
#include "ui_publisherprofilepage.h"

Publisherprofilepage::Publisherprofilepage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Publisherprofilepage)
{
    ui->setupUi(this);
}

Publisherprofilepage::~Publisherprofilepage()
{
    delete ui;
}

#include "cartitemwidget.h"
#include "ui_cartitemwidget.h"

CartitemWidget::CartitemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CartitemWidget)
{
    ui->setupUi(this);
}

CartitemWidget::~CartitemWidget()
{
    delete ui;
}

#include "storebookwidget.h"
#include "ui_storebookwidget.h"

StorebookWidget::StorebookWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StorebookWidget)
{
    ui->setupUi(this);
}

StorebookWidget::~StorebookWidget()
{
    delete ui;
}

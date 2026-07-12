#include "pdfviewerwidget.h"
#include "ui_pdfviewerwidget.h"

pdfviewerWidget::pdfviewerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::pdfviewerWidget)
{
    ui->setupUi(this);
}

pdfviewerWidget::~pdfviewerWidget()
{
    delete ui;
}

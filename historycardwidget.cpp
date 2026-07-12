#include "historycardwidget.h"
#include "ui_historycardwidget.h"

HistoryCardwidget::HistoryCardwidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HistoryCardwidget)
{
    ui->setupUi(this);
}

HistoryCardwidget::~HistoryCardwidget()
{
    delete ui;
}

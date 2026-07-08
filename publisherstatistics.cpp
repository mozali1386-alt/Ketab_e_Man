#include "publisherstatistics.h"
#include "ui_publisherstatistics.h"

PublisherStatistics::PublisherStatistics(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PublisherStatistics)
{
    ui->setupUi(this);
    ui->tableWidget_maximum->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_minimum->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

PublisherStatistics::~PublisherStatistics()
{
    delete ui;
}

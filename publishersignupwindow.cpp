#include "publishersignupwindow.h"
#include "ui_publishersignupwindow.h"

publishersignupwindow::publishersignupwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::publishersignupwindow)
{
    ui->setupUi(this);
}

publishersignupwindow::~publishersignupwindow()
{
    delete ui;
}

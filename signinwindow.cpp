#include "signinwindow.h"
#include "ui_signinwindow.h"
signinwindow::signinwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::signinwindow)
{
    ui->setupUi(this);
}

signinwindow::~signinwindow()
{
    delete ui;
}

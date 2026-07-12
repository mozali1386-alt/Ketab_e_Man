#include "usersignupwindow.h"
#include "ui_usersignupwindow.h"

Usersignupwindow::Usersignupwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Usersignupwindow)
{
    ui->setupUi(this);
}

Usersignupwindow::~Usersignupwindow()
{
    delete ui;
}

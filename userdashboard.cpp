#include "userdashboard.h"
#include "ui_userdashboard.h"

UserDashboard::UserDashboard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UserDashboard)
{
    ui->setupUi(this);
}

UserDashboard::~UserDashboard()
{
    delete ui;
}

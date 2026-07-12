#include "publisherdashboard.h"
#include "ui_publisherdashboard.h"

Publisherdashboard::Publisherdashboard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Publisherdashboard)
{
    ui->setupUi(this);
}

Publisherdashboard::~Publisherdashboard()
{
    delete ui;
}

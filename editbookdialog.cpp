#include "editbookdialog.h"
#include "ui_editbookdialog.h"

EditbookDialog::EditbookDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EditbookDialog)
{
    ui->setupUi(this);
}

EditbookDialog::~EditbookDialog()
{
    delete ui;
}

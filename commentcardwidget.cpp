#include "commentcardwidget.h"
#include "ui_commentcardwidget.h"

CommentcardWidget::CommentcardWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CommentcardWidget)
{
    ui->setupUi(this);
}

CommentcardWidget::~CommentcardWidget()
{
    delete ui;
}

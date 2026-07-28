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

void CommentcardWidget::setCommentData(const QString &fullName,
                                       int score,
                                       const QString &commentText,
                                       const QString &date)
{
    ui->label_nameuser->setText(fullName);
    ui->label_tarikh->setText(date);
    ui->label_comment->setText(commentText);

    QString starsHtml = "<div dir='ltr'>";
    for (int i = 1; i <= 5; ++i) {
        if (i <= score) {
            starsHtml += "<img src=':/images/icons8-star-48 (1).png' width='16' height='16'>";
        } else {
            starsHtml += "<img src=':/images/icons8-star-48 (2).png' width='16' height='16'>";
        }
    }
    starsHtml += "</div>";

    ui->label_scorestar->setText(starsHtml);
}
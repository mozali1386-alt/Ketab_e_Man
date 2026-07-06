#ifndef COMMENTCARDWIDGET_H
#define COMMENTCARDWIDGET_H

#include <QWidget>

namespace Ui {
class CommentcardWidget;
}

class CommentcardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CommentcardWidget(QWidget *parent = nullptr);
    ~CommentcardWidget();

private:
    Ui::CommentcardWidget *ui;
};

#endif // COMMENTCARDWIDGET_H

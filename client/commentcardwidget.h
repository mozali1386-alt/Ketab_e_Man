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

    void setCommentData(const QString &fullName,
                        int score,
                        const QString &commentText,
                        const QString &date);

private:
    Ui::CommentcardWidget *ui;
};

#endif // COMMENTCARDWIDGET_H
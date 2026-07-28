#ifndef STOREBOOKWIDGET_H
#define STOREBOOKWIDGET_H

#include <QMouseEvent>
#include <QWidget>

namespace Ui {
class StorebookWidget;
}

class StorebookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StorebookWidget(QWidget *parent = nullptr);
    ~StorebookWidget();

    void setBookData(const QString &image,
                     const QString &title,
                     const QString &author,
                     const QString &finalPrice,
                     const QString &score);

    void setBookId(const QString &id);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:

    void bookClicked(QString bookId);

private:
    Ui::StorebookWidget *ui;

    QString currentBookId;
};

#endif
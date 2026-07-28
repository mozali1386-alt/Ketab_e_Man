#ifndef HISTORYCARDWIDGET_H
#define HISTORYCARDWIDGET_H

#include <QWidget>

namespace Ui {
class HistoryCardwidget;
}

class HistoryCardwidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryCardwidget(QWidget *parent = nullptr);
    ~HistoryCardwidget();

    void setBookData(QString bookName, QString authorName, QString price, QString date);

private:
    Ui::HistoryCardwidget *ui;
};

#endif // HISTORYCARDWIDGET_H
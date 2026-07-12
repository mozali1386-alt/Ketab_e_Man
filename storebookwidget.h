#ifndef STOREBOOKWIDGET_H
#define STOREBOOKWIDGET_H

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

private:
    Ui::StorebookWidget *ui;
};

#endif // STOREBOOKWIDGET_H

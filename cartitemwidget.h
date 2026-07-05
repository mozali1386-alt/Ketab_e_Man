#ifndef CARTITEMWIDGET_H
#define CARTITEMWIDGET_H

#include <QWidget>

namespace Ui {
class CartitemWidget;
}

class CartitemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CartitemWidget(QWidget *parent = nullptr);
    ~CartitemWidget();

private:
    Ui::CartitemWidget *ui;
};

#endif // CARTITEMWIDGET_H

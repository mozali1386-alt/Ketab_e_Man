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

    void setBookData(const QString &image,
                     const QString &title,
                     const QString &author,
                     int mainPrice,
                     int discount);

    bool isChecked();
    void setChecked(bool state);

    int getMainPrice();
    int getOffPrice();
    int getFinalPrice();

signals:
    void itemDeleted(CartitemWidget *item);
    void itemToggled();

private slots:
    void on_toolButton_trash_clicked();
    void on_checkBox_choose_clicked();

private:
    Ui::CartitemWidget *ui;

    int mainPriceValue;
    int offPriceValue;
    int finalPriceValue;
};

#endif // CARTITEMWIDGET_H
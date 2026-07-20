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

    void setBookData(const QString &id,
                     const QString &imagePath,
                     const QString &title,
                     const QString &author,
                     int mainPrice,
                     int discount);

    QString getBookId() const;
    int getMainPrice() const;
    int getOffPrice() const;
    int getFinalPrice() const;

signals:
    void itemDeleted(CartitemWidget *item);

private slots:
    void on_toolButton_trash_clicked();

private:
    Ui::CartitemWidget *ui;

    QString currentBookId;
    int mainPriceValue;
    int offPriceValue;
    int finalPriceValue;
};

#endif // CARTITEMWIDGET_H
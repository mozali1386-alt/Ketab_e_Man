#ifndef CARTPAGE_H
#define CARTPAGE_H

#include <QWidget>
#include "cartitemwidget.h"

namespace Ui {
class Cartpage;
}

class Cartpage : public QWidget
{
    Q_OBJECT

public:
    explicit Cartpage(QWidget *parent = nullptr);
    ~Cartpage();

private slots:
    void updateSummary();
    void removeBookFromCart(CartitemWidget *bookToRemove);
    void on_checkBox_checkall_clicked();

private:
    Ui::Cartpage *ui;

    void addNewBookToCart(CartitemWidget *newBook);
    void loadCartItems(const QString &serverResponse); // تابع پردازش رشته سرور
};

#endif // CARTPAGE_H
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

    void requestCartItems();
    void requestCartBookSummary(const QString &bookId);

private slots:
    void updateSummary();
    void removeBookFromCart(CartitemWidget *bookToRemove);
    void on_pushButton_sabt_clicked();
    void processServerResponse(const QString &response);

private:
    Ui::Cartpage *ui;

    void addNewBookToCart(CartitemWidget *newBook);
    void clearCartUI();

    int expectedCartItems = 0;
    int loadedCartItems = 0;
    int savedOldPrice = 0;
    bool isCheckingOut = false;
};

#endif // CARTPAGE_H

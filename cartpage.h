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
    void processServerResponse(const QString &response);

private slots:
    void updateSummary();
    void removeBookFromCart(CartitemWidget *bookToRemove);
    void on_pushButton_sabt_clicked();

private:
    Ui::Cartpage *ui;

    void addNewBookToCart(CartitemWidget *newBook);
    void clearCartUI();
};

#endif // CARTPAGE_H
//in the name of ALLAH
//YA MAHDI

#ifndef CARTITEM_H
#define CARTITEM_H

#include <QString>

class CartItem {
public:
    CartItem();

    CartItem(quint64 bookId, int quantity);

    quint64 getBookId() const;

    int getQuantity() const;

    void setBookId(quint64 newBookId);

    void setQuantity(int newQuantity);

    QString serialize() const;

    void deserialize(const QString &data);

private:
    quint64 bookId;
    int quantity;
};

#endif

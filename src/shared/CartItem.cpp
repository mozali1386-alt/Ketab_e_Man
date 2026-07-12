//in the name of ALLAH
//YA MAHDI

#include "CartItem.h"
#include <QStringList>

CartItem::CartItem() {
    bookId = 0;
    quantity = 1;
}

CartItem::CartItem(quint64 bookId, int quantity) {
    this->bookId = bookId;
    this->quantity = quantity;
}

quint64 CartItem::getBookId() const {
    return bookId;
}

int CartItem::getQuantity() const {
    return quantity;
}

void CartItem::setBookId(quint64 newBookId) {
    bookId = newBookId;
}

void CartItem::setQuantity(int newQuantity) {
    if (newQuantity < 1) {
        newQuantity = 1;
    }
    quantity = newQuantity;
}

QString CartItem::serialize() const {
    QString result = QString::number(bookId) + ":" + QString::number(quantity);
    return result;
}

void CartItem::deserialize(const QString &data) {
    QStringList parts = data.split(":");
    bookId = parts.at(0).toULongLong();
    quantity = parts.at(1).toInt();
}

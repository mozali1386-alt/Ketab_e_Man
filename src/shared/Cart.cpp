//in the name of ALLAH
//YA MAHDI

#include "Cart.h"
#include <QStringList>

Cart::Cart() {
    ownerId = 0;
}

Cart::~Cart() {
}

quint64 Cart::getOwnerId() const {
    return ownerId;
}

QVector<CartItem> Cart::getItems() const {
    return items;
}

void Cart::setOwnerId(quint64 newOwnerId) {
    ownerId = newOwnerId;
    touchUpdatedAt();
}

void Cart::addItem(quint64 bookId, int quantity) {
    for (int i = 0; i < items.size(); i++) {
        if (items.at(i).getBookId() == bookId) {
            int currentQuantity = items.at(i).getQuantity();
            items[i].setQuantity(currentQuantity + quantity);
            touchUpdatedAt();
            return;
        }
    }

    CartItem newItem(bookId, quantity);
    items.append(newItem);
    touchUpdatedAt();
}

void Cart::removeItem(quint64 bookId) {
    for (int i = 0; i < items.size(); i++) {
        if (items.at(i).getBookId() == bookId) {
            items.remove(i);
            break;
        }
    }
    touchUpdatedAt();
}

void Cart::clear() {
    items.clear();
    touchUpdatedAt();
}

quint64 Cart::generateId() {
    return BaseEntity::generateId();
}

QString Cart::serialize() const {
    QStringList itemList;
    for (int i = 0; i < items.size(); i++) {
        itemList.append(items.at(i).serialize());
    }

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(ownerId) + "|";
    result += itemList.join(";");

    return result;
}

void Cart::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    ownerId = tokens.at(index).toULongLong();
    index++;

    items.clear();
    QString itemsToken = tokens.at(index);
    index++;
    if (itemsToken.length() > 0) {
        QStringList itemParts = itemsToken.split(";");
        for (int i = 0; i < itemParts.size(); i++) {
            CartItem newItem;
            newItem.deserialize(itemParts.at(i));
            items.append(newItem);
        }
    }
}

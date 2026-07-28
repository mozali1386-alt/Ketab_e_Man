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

QSet<quint64> Cart::getBookIds() const {
    return bookIds;
}

void Cart::setOwnerId(quint64 newOwnerId) {
    ownerId = newOwnerId;
    touchUpdatedAt();
}

void Cart::addItem(quint64 bookId) {
    bookIds.insert(bookId);
    touchUpdatedAt();
}

void Cart::removeItem(quint64 bookId) {
    bookIds.remove(bookId);
    touchUpdatedAt();
}

void Cart::clear() {
    bookIds.clear();
    touchUpdatedAt();
}

quint64 Cart::generateId() {
    return BaseEntity::generateId();
}

QString Cart::serialize() const {
    QStringList itemList;
    for (quint64 bookId: bookIds) {
        itemList.append(QString::number(bookId));
    }

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(ownerId) + "|";
    result += itemList.join(",");

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

    bookIds.clear();
    QString itemsToken = tokens.at(index);
    index++;
    if (itemsToken.length() > 0) {
        QStringList parts = itemsToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            bookIds.insert(parts.at(i).toULongLong());
        }
    }
}

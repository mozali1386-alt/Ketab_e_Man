//in the name of ALLAH
//YA MAHDI

#include "Purchase.h"
#include <QStringList>

Purchase::Purchase() {
    buyerId = 0;
    totalAmount = 0.0;
    purchaseDate = QDateTime::currentDateTime();
}

Purchase::~Purchase() {
}

quint64 Purchase::getBuyerId() const {
    return buyerId;
}

QVector<quint64> Purchase::getBookIds() const {
    return bookIds;
}

double Purchase::getTotalAmount() const {
    return totalAmount;
}

QDateTime Purchase::getPurchaseDate() const {
    return purchaseDate;
}

void Purchase::setBuyerId(quint64 newBuyerId) {
    buyerId = newBuyerId;
    touchUpdatedAt();
}

void Purchase::setBookIds(const QVector<quint64> &newBookIds) {
    bookIds = newBookIds;
    touchUpdatedAt();
}

void Purchase::setTotalAmount(double newTotalAmount) {
    totalAmount = newTotalAmount;
    touchUpdatedAt();
}

quint64 Purchase::generateId() {
    return BaseEntity::generateId();
}

QString Purchase::serialize() const {
    QStringList bookList;
    for (int i = 0; i < bookIds.size(); i++) {
        bookList.append(QString::number(bookIds.at(i)));
    }

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(buyerId) + "|";
    result += bookList.join(",") + "|";
    result += QString::number(totalAmount) + "|";
    result += purchaseDate.toString(Qt::ISODate);

    return result;
}

void Purchase::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    buyerId = tokens.at(index).toULongLong();
    index++;

    bookIds.clear();
    QString bookToken = tokens.at(index);
    index++;
    if (bookToken.length() > 0) {
        QStringList parts = bookToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            bookIds.append(parts.at(i).toULongLong());
        }
    }

    totalAmount = tokens.at(index).toDouble();
    index++;
    purchaseDate = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
}

//in the name of ALLAH
//YA MAHDI

#include "Transaction.h"
#include <QStringList>

Transaction::Transaction() {
    walletId = 0;
    amount = 0.0;
    type = TransactionType::DEPOSIT;
    bookPriceAtPurchase = QPair<quint64, double>(0, 0.0);
}

Transaction::~Transaction() {
}

quint64 Transaction::getWalletId() const {
    return walletId;
}

double Transaction::getAmount() const {
    return amount;
}

TransactionType Transaction::getType() const {
    return type;
}

QPair<quint64, double> Transaction::getBookPriceAtPurchase() const {
    return bookPriceAtPurchase;
}

void Transaction::setWalletId(quint64 newWalletId) {
    walletId = newWalletId;
    touchUpdatedAt();
}

void Transaction::setAmount(double newAmount) {
    amount = newAmount;
    touchUpdatedAt();
}

void Transaction::setType(TransactionType newType) {
    type = newType;
    touchUpdatedAt();
}

void Transaction::setBookPriceAtPurchase(quint64 bookId, double priceAtPurchase) {
    bookPriceAtPurchase = QPair<quint64, double>(bookId, priceAtPurchase);
    touchUpdatedAt();
}

quint64 Transaction::generateId() {
    return BaseEntity::generateId();
}

QString Transaction::serialize() const {
    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(walletId) + "|";
    result += QString::number(amount) + "|";
    result += QString::number(static_cast<int>(type)) + "|";
    result += QString::number(bookPriceAtPurchase.first) + ":" + QString::number(bookPriceAtPurchase.second);

    return result;
}

void Transaction::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    walletId = tokens.at(index).toULongLong();
    index++;
    amount = tokens.at(index).toDouble();
    index++;
    type = static_cast<TransactionType>(tokens.at(index).toInt());
    index++;

    QStringList bookPriceParts = tokens.at(index).split(":");
    index++;
    quint64 bookId = bookPriceParts.at(0).toULongLong();
    double priceAtPurchase = bookPriceParts.at(1).toDouble();
    bookPriceAtPurchase = QPair<quint64, double>(bookId, priceAtPurchase);
}

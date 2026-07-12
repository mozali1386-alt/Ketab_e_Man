//in the name of ALLAH
//YA MAHDI

#include "Transaction.h"
#include <QStringList>

Transaction::Transaction() {
    walletId = 0;
    amount = 0.0;
    type = TransactionType::DEPOSIT;
    description = "";
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

QString Transaction::getDescription() const {
    return description;
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

void Transaction::setDescription(const QString &newDescription) {
    description = newDescription;
    touchUpdatedAt();
}

quint64 Transaction::generateId() {
    return BaseEntity::generateId();
}

QString Transaction::serialize() const {
    QString safeDescription = description;
    safeDescription.replace("|", "&pipe;");

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(walletId) + "|";
    result += QString::number(amount) + "|";
    result += QString::number(static_cast<int>(type)) + "|";
    result += safeDescription;

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

    description = tokens.at(index);
    index++;
    description.replace("&pipe;", "|");
}

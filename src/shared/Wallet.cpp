//in the name of ALLAH
//YA MAHDI

#include "Wallet.h"
#include <QStringList>

Wallet::Wallet() {
    ownerId = 0;
    balance = 0.0;
}

Wallet::~Wallet() {
}

quint64 Wallet::getOwnerId() const {
    return ownerId;
}

double Wallet::getBalance() const {
    return balance;
}

QVector<quint64> Wallet::getTransactionIds() const {
    return transactionIds;
}

void Wallet::setOwnerId(quint64 newOwnerId) {
    ownerId = newOwnerId;
    touchUpdatedAt();
}

void Wallet::deposit(double amount) {
    if (amount <= 0.0) {
        return;
    }

    balance = balance + amount;
    touchUpdatedAt();
}

bool Wallet::withdraw(double amount) {
    if (amount <= 0.0) {
        return false;
    }

    if (balance < amount) {
        return false;
    }

    balance = balance - amount;
    touchUpdatedAt();
    return true;
}

void Wallet::addTransaction(quint64 transactionId) {
    transactionIds.append(transactionId);
    touchUpdatedAt();
}

quint64 Wallet::generateId() {
    return BaseEntity::generateId();
}

QString Wallet::serialize() const {
    QStringList transactionList;
    for (int i = 0; i < transactionIds.size(); i++) {
        transactionList.append(QString::number(transactionIds.at(i)));
    }

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(ownerId) + "|";
    result += QString::number(balance) + "|";
    result += transactionList.join(",");

    return result;
}

void Wallet::deserialize(const QString &data) {
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
    balance = tokens.at(index).toDouble();
    index++;

    transactionIds.clear();
    QString transactionToken = tokens.at(index);
    index++;
    if (transactionToken.length() > 0) {
        QStringList parts = transactionToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            transactionIds.append(parts.at(i).toULongLong());
        }
    }
}

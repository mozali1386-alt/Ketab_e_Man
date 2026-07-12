//in the name of ALLAH
//YA MAHDI

#ifndef WALLET_H
#define WALLET_H

#include "BaseEntity.h"
#include <QVector>

class Wallet : public BaseEntity {
public:
    Wallet();

    virtual ~Wallet();

    quint64 getOwnerId() const;

    double getBalance() const;

    QVector<quint64> getTransactionIds() const;

    void setOwnerId(quint64 newOwnerId);

    void deposit(double amount);

    bool withdraw(double amount);

    void addTransaction(quint64 transactionId);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 ownerId;
    double balance;
    QVector<quint64> transactionIds;
};

#endif

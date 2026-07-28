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

    qint64 getBalance() const;

    QVector<quint64> getTransactionIds() const;

    void setOwnerId(quint64 newOwnerId);

    void deposit(qint64 amount);

    bool withdraw(qint64 amount);

    void addTransaction(quint64 transactionId);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 ownerId;
    qint64 balance;
    QVector<quint64> transactionIds;
};

#endif

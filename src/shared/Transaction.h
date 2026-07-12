//in the name of ALLAH
//YA MAHDI

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "BaseEntity.h"
#include "Enums.h"
#include <QString>

class Transaction : public BaseEntity {
public:
    Transaction();

    virtual ~Transaction();

    quint64 getWalletId() const;

    double getAmount() const;

    TransactionType getType() const;

    void setWalletId(quint64 newWalletId);

    void setAmount(double newAmount);

    void setType(TransactionType newType);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 walletId;
    double amount;
    TransactionType type;
};

#endif

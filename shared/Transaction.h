//in the name of ALLAH
//YA MAHDI

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "BaseEntity.h"
#include "Enums.h"
#include <QString>
#include <QPair>

class Transaction : public BaseEntity {
public:
    Transaction();

    virtual ~Transaction();

    quint64 getWalletId() const;

    qint64 getAmount() const;

    TransactionType getType() const;

    QPair<quint64, qint64> getBookPriceAtPurchase() const;

    void setWalletId(quint64 newWalletId);

    void setAmount(qint64 newAmount);

    void setType(TransactionType newType);

    void setBookPriceAtPurchase(quint64 bookId, qint64 priceAtPurchase);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 walletId;
    qint64 amount;
    TransactionType type;
    QPair<quint64, qint64> bookPriceAtPurchase;
};

#endif

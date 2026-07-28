//in the name of ALLAH
//YA MAHDI

#ifndef PURCHASE_H
#define PURCHASE_H

#include "BaseEntity.h"
#include <QSet>

class Purchase : public BaseEntity {
public:
    Purchase();

    virtual ~Purchase();

    quint64 getBuyerId() const;

    QSet<quint64> getBookIds() const;

    qint64 getTotalAmount() const;

    QDateTime getPurchaseDate() const;

    void setBuyerId(quint64 newBuyerId);

    void setBookIds(const QSet<quint64> &newBookIds);

    void setTotalAmount(qint64 newTotalAmount);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 buyerId;
    QSet<quint64> bookIds;
    qint64 totalAmount;
    QDateTime purchaseDate;
};

#endif

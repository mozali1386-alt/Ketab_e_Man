//in the name of ALLAH
//YA MAHDI

#ifndef PURCHASE_H
#define PURCHASE_H

#include "BaseEntity.h"
#include <QVector>

class Purchase : public BaseEntity {
public:
    Purchase();

    virtual ~Purchase();

    quint64 getBuyerId() const;

    QVector<quint64> getBookIds() const;

    double getTotalAmount() const;

    QDateTime getPurchaseDate() const;

    void setBuyerId(quint64 newBuyerId);

    void setBookIds(const QVector<quint64> &newBookIds);

    void setTotalAmount(double newTotalAmount);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 buyerId;
    QVector<quint64> bookIds;
    double totalAmount;
    QDateTime purchaseDate;
};

#endif

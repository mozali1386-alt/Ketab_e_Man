//in the name of ALLAH
//YA MAHDI

#ifndef CART_H
#define CART_H

#include "BaseEntity.h"
#include <QSet>

class Cart : public BaseEntity {
public:
    Cart();

    virtual ~Cart();

    quint64 getOwnerId() const;

    QSet<quint64> getBookIds() const;

    void setOwnerId(quint64 newOwnerId);

    void addItem(quint64 bookId);

    void removeItem(quint64 bookId);

    void clear();

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 ownerId;
    QSet<quint64> bookIds;
};

#endif

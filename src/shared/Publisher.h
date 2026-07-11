//in the name of ALLAH
//YA MAHDI

#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "User.h"
#include <QVector>

class Publisher : public User {
public:
    Publisher();

    virtual ~Publisher();

    QVector<quint64> getMyBookIds() const;

    double getRevenue() const;

    void publishBook(quint64 bookId);

    void removeBook(quint64 bookId);

    void receiveSaleIncome(double amount);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    QVector<quint64> myBookIds;
    double revenue;
};

#endif

//in the name of ALLAH
//YA MAHDI

#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "User.h"
#include <QSet>

class Publisher : public User {
public:
    Publisher();

    virtual ~Publisher();

    QSet<quint64> getMyBookIds() const;

    double getRevenue() const;

    QString getBio() const;

    void publishBook(quint64 bookId);

    void removeBook(quint64 bookId);

    void receiveSaleIncome(double amount);

    void setBio(const QString &newBio);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    QSet<quint64> myBookIds;
    double revenue;
    QString bio;
};

#endif

//in the name of ALLAH
//YA MAHDI

#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include <QVector>

class Admin : public User {
public:
    Admin();

    virtual ~Admin();

    void blockUser(quint64 userId);

    void unblockUser(quint64 userId);

    void deleteUser(quint64 userId);

    void activateUserAccount(quint64 userId);

    void deleteBook(quint64 bookId);

    void deleteReview(quint64 reviewId);

    QVector<quint64> viewAllUsers() const;

    QVector<quint64> viewAllBooks() const;

    void setAllUserIdsCache(const QVector<quint64> &userIds);

    void setAllBookIdsCache(const QVector<quint64> &bookIds);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    QVector<quint64> cachedUserIds;
    QVector<quint64> cachedBookIds;
};

#endif

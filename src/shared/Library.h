//in the name of ALLAH
//YA MAHDI

#ifndef LIBRARY_H
#define LIBRARY_H

#include "BaseEntity.h"
#include <QSet>

class Library : public BaseEntity {
public:
    Library();

    virtual ~Library();

    quint64 getOwnerId() const;

    QSet<quint64> getShelves() const;

    QSet<quint64> getPurchasedBooks() const;

    QSet<quint64> getSavedBooks() const;

    void setOwnerId(quint64 newOwnerId);

    void addShelf(quint64 shelfId);

    void removeShelf(quint64 shelfId);

    void addToSavedBooks(quint64 bookId);

    void removeFromSavedBooks(quint64 bookId);

    void addToPurchasedBooks(quint64 bookId);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 ownerId;
    QSet<quint64> shelfIds;
    QSet<quint64> purchasedBookIds;
    QSet<quint64> savedBookIds;
};

#endif

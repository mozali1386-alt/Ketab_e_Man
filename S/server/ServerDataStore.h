//in the name of ALLAH
//YA MAHDI

#ifndef SERVERDATASTORE_H
#define SERVERDATASTORE_H

#include <QMap>
#include <QHash>
#include "../shared/User.h"
#include "../shared/Book.h"
#include "../shared/Author.h"
#include "../shared/Library.h"
#include "../shared/Shelf.h"
#include "../shared/Review.h"
#include "../shared/Cart.h"
#include "../shared/Notification.h"
#include "../shared/Wallet.h"
#include "../shared/Transaction.h"
#include "../shared/Purchase.h"

class ServerDataStore
{
public:
    ServerDataStore();
    ~ServerDataStore();

    QMap<quint64, User*>& getUsersMap();
    QMap<quint64, Book*>& getBooksMap();
    QMap<quint64, Author*>& getAuthorsMap();
    QMap<quint64, Library*>& getLibrariesMap();
    QMap<quint64, Shelf*>& getShelvesMap();
    QMap<quint64, Review*>& getReviewsMap();
    QMap<quint64, Cart*>& getCartsMap();
    QMap<quint64, Notification*>& getNotificationsMap();
    QMap<quint64, Wallet*>& getWalletsMap();
    QMap<quint64, Transaction*>& getTransactionsMap();
    QMap<quint64, Purchase*>& getPurchasesMap();

    User* findUserByUsername(const QString &username) const;
    void registerUsername(const QString &username, quint64 userId);

    User* findUserByEmail(const QString &email) const;
    void registerEmail(const QString &email, quint64 userId);
    void unregisterEmail(const QString &email);

    Cart* findCartByOwner(quint64 ownerId) const;
    void registerCartOwner(quint64 ownerId, quint64 cartId);

    Author* findAuthorByName(const QString &authorName) const;
    void registerAuthorName(const QString &authorName, quint64 authorId);

    void rebuildAllIndexes();

private:
    QMap<quint64, User*> users;
    QMap<quint64, Book*> books;
    QMap<quint64, Author*> authors;
    QMap<quint64, Library*> libraries;
    QMap<quint64, Shelf*> shelves;
    QMap<quint64, Review*> reviews;
    QMap<quint64, Cart*> carts;
    QMap<quint64, Notification*> notifications;
    QMap<quint64, Wallet*> wallets;
    QMap<quint64, Transaction*> transactions;
    QMap<quint64, Purchase*> purchases;
    QHash<QString, quint64> usernameIndex;
    QHash<QString, quint64> emailIndex;
    QHash<quint64, quint64> cartOwnerIndex;
    QHash<QString, quint64> authorNameIndex;
};

#endif

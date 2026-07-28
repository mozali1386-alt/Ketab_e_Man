//in the name of ALLAH
//YA MAHDI

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QMap>
#include "../../shared/User.h"
#include "../../shared/Book.h"
#include "../../shared/Author.h"
#include "../../shared/Library.h"
#include "../../shared/Shelf.h"
#include "../../shared/Review.h"
#include "../../shared/Cart.h"
#include "../../shared/Notification.h"
#include "../../shared/Wallet.h"
#include "../../shared/Transaction.h"
#include "../../shared/Purchase.h"

class ServerDataStore;

class FileManager {
public:
    FileManager();

    bool saveAllData(ServerDataStore *dataStore);

    bool loadAllData(ServerDataStore *dataStore);

private:
    QString baseDirectory;

    void ensureBaseDirectoryExists();

    void saveUsers(QMap<quint64, User *> &users);

    void loadUsers(QMap<quint64, User *> &users);

    void saveBooks(QMap<quint64, Book *> &books);

    void loadBooks(QMap<quint64, Book *> &books);

    void saveAuthors(QMap<quint64, Author *> &authors);

    void loadAuthors(QMap<quint64, Author *> &authors);

    void saveLibraries(QMap<quint64, Library *> &libraries);

    void loadLibraries(QMap<quint64, Library *> &libraries);

    void saveShelves(QMap<quint64, Shelf *> &shelves);

    void loadShelves(QMap<quint64, Shelf *> &shelves);

    void saveReviews(QMap<quint64, Review *> &reviews);

    void loadReviews(QMap<quint64, Review *> &reviews);

    void saveCarts(QMap<quint64, Cart *> &carts);

    void loadCarts(QMap<quint64, Cart *> &carts);

    void saveNotifications(QMap<quint64, Notification *> &notifications);

    void loadNotifications(QMap<quint64, Notification *> &notifications);

    void saveWallets(QMap<quint64, Wallet *> &wallets);

    void loadWallets(QMap<quint64, Wallet *> &wallets);

    void saveTransactions(QMap<quint64, Transaction *> &transactions);

    void loadTransactions(QMap<quint64, Transaction *> &transactions);

    void savePurchases(QMap<quint64, Purchase *> &purchases);

    void loadPurchases(QMap<quint64, Purchase *> &purchases);
};

#endif

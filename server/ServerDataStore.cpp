//in the name of ALLAH
//YA MAHDI

#include "ServerDataStore.h"

ServerDataStore::ServerDataStore() {
}

ServerDataStore::~ServerDataStore() {
    qDeleteAll(users);
    qDeleteAll(books);
    qDeleteAll(authors);
    qDeleteAll(libraries);
    qDeleteAll(shelves);
    qDeleteAll(reviews);
    qDeleteAll(carts);
    qDeleteAll(notifications);
    qDeleteAll(wallets);
    qDeleteAll(transactions);
    qDeleteAll(purchases);
}

QMap<quint64, User *> &ServerDataStore::getUsersMap() {
    return users;
}

QMap<quint64, Book *> &ServerDataStore::getBooksMap() {
    return books;
}

QMap<quint64, Author *> &ServerDataStore::getAuthorsMap() {
    return authors;
}

QMap<quint64, Library *> &ServerDataStore::getLibrariesMap() {
    return libraries;
}

QMap<quint64, Shelf *> &ServerDataStore::getShelvesMap() {
    return shelves;
}

QMap<quint64, Review *> &ServerDataStore::getReviewsMap() {
    return reviews;
}

QMap<quint64, Cart *> &ServerDataStore::getCartsMap() {
    return carts;
}

QMap<quint64, Notification *> &ServerDataStore::getNotificationsMap() {
    return notifications;
}

QMap<quint64, Wallet *> &ServerDataStore::getWalletsMap() {
    return wallets;
}

QMap<quint64, Transaction *> &ServerDataStore::getTransactionsMap() {
    return transactions;
}

QMap<quint64, Purchase *> &ServerDataStore::getPurchasesMap() {
    return purchases;
}

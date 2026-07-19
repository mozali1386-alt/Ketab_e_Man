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

User *ServerDataStore::findUserByUsername(const QString &username) const {
    if (!usernameIndex.contains(username)) {
        return nullptr;
    }
    quint64 userId = usernameIndex.value(username);
    return users.value(userId, nullptr);
}

void ServerDataStore::registerUsername(const QString &username, quint64 userId) {
    usernameIndex.insert(username, userId);
}

User *ServerDataStore::findUserByEmail(const QString &email) const {
    if (!emailIndex.contains(email)) {
        return nullptr;
    }
    quint64 userId = emailIndex.value(email);
    return users.value(userId, nullptr);
}

void ServerDataStore::registerEmail(const QString &email, quint64 userId) {
    emailIndex.insert(email, userId);
}

Cart *ServerDataStore::findCartByOwner(quint64 ownerId) const {
    if (!cartOwnerIndex.contains(ownerId)) {
        return nullptr;
    }
    quint64 cartId = cartOwnerIndex.value(ownerId);
    return carts.value(cartId, nullptr);
}

void ServerDataStore::registerCartOwner(quint64 ownerId, quint64 cartId) {
    cartOwnerIndex.insert(ownerId, cartId);
}

Author *ServerDataStore::findAuthorByName(const QString &authorName) const {
    if (!authorNameIndex.contains(authorName)) {
        return nullptr;
    }
    quint64 authorId = authorNameIndex.value(authorName);
    return authors.value(authorId, nullptr);
}

void ServerDataStore::registerAuthorName(const QString &authorName, quint64 authorId) {
    authorNameIndex.insert(authorName, authorId);
}

void ServerDataStore::rebuildAllIndexes() {
    usernameIndex.clear();
    emailIndex.clear();
    QList<quint64> userKeys = users.keys();
    for (int i = 0; i < userKeys.size(); i++) {
        User *user = users.value(userKeys.at(i));
        if (user != nullptr) {
            usernameIndex.insert(user->getUsername(), user->getId());
            emailIndex.insert(user->getEmail(), user->getId());
        }
    }

    cartOwnerIndex.clear();
    QList<quint64> cartKeys = carts.keys();
    for (int i = 0; i < cartKeys.size(); i++) {
        Cart *cart = carts.value(cartKeys.at(i));
        if (cart != nullptr) {
            cartOwnerIndex.insert(cart->getOwnerId(), cart->getId());
        }
    }

    authorNameIndex.clear();
    QList<quint64> authorKeys = authors.keys();
    for (int i = 0; i < authorKeys.size(); i++) {
        Author *author = authors.value(authorKeys.at(i));
        if (author != nullptr) {
            authorNameIndex.insert(author->getFullName(), author->getId());
        }
    }
}

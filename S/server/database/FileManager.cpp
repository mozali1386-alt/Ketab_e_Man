//in the name of ALLAH
//YA MAHDI

#include "FileManager.h"
#include "../ServerDataStore.h"
#include "../../shared/NormalUser.h"
#include "../../shared/Publisher.h"
#include "../../shared/Admin.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>
#include <QCoreApplication>

FileManager::FileManager() {
    baseDirectory = QCoreApplication::applicationDirPath() + "/assets/data/";
}

void FileManager::ensureBaseDirectoryExists() {
    QDir dir;
    dir.mkpath(baseDirectory);
}

bool FileManager::saveAllData(ServerDataStore *dataStore) {
    if (dataStore == nullptr) {
        return false;
    }

    ensureBaseDirectoryExists();

    saveUsers(dataStore->getUsersMap());
    saveBooks(dataStore->getBooksMap());
    saveAuthors(dataStore->getAuthorsMap());
    saveLibraries(dataStore->getLibrariesMap());
    saveShelves(dataStore->getShelvesMap());
    saveReviews(dataStore->getReviewsMap());
    saveCarts(dataStore->getCartsMap());
    saveNotifications(dataStore->getNotificationsMap());
    saveWallets(dataStore->getWalletsMap());
    saveTransactions(dataStore->getTransactionsMap());
    savePurchases(dataStore->getPurchasesMap());

    return true;
}

bool FileManager::loadAllData(ServerDataStore *dataStore) {
    if (dataStore == nullptr) {
        return false;
    }

    ensureBaseDirectoryExists();

    loadUsers(dataStore->getUsersMap());
    loadBooks(dataStore->getBooksMap());
    loadAuthors(dataStore->getAuthorsMap());
    loadLibraries(dataStore->getLibrariesMap());
    loadShelves(dataStore->getShelvesMap());
    loadReviews(dataStore->getReviewsMap());
    loadCarts(dataStore->getCartsMap());
    loadNotifications(dataStore->getNotificationsMap());
    loadWallets(dataStore->getWalletsMap());
    loadTransactions(dataStore->getTransactionsMap());
    loadPurchases(dataStore->getPurchasesMap());

    return true;
}

void FileManager::saveUsers(QMap<quint64, User *> &users) {
    QFile file(baseDirectory + "users.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = users.keys();
    for (int i = 0; i < keys.size(); i++) {
        User *user = users.value(keys.at(i));
        if (user == nullptr) {
            continue;
        }
        out << user->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadUsers(QMap<quint64, User *> &users) {
    QFile file(baseDirectory + "users.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        QStringList tokens = line.split("|");
        int roleInt = tokens.at(3).toInt();

        User *user = nullptr;
        if (roleInt == static_cast<int>(Role::PUBLISHER)) {
            user = new Publisher();
        } else if (roleInt == static_cast<int>(Role::ADMIN)) {
            user = new Admin();
        } else {
            user = new NormalUser();
        }

        user->deserialize(line);
        BaseEntity::registerExistingId(user->getId());
        if (users.contains(user->getId())) {
            delete users.value(user->getId());
        }
        users.insert(user->getId(), user);
    }

    file.close();
}

void FileManager::saveBooks(QMap<quint64, Book *> &books) {
    QFile file(baseDirectory + "books.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = books.keys();
    for (int i = 0; i < keys.size(); i++) {
        Book *book = books.value(keys.at(i));
        if (book == nullptr) {
            continue;
        }
        out << book->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadBooks(QMap<quint64, Book *> &books) {
    QFile file(baseDirectory + "books.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Book *book = new Book();
        book->deserialize(line);
        BaseEntity::registerExistingId(book->getId());
        if (books.contains(book->getId())) {
            delete books.value(book->getId());
        }
        books.insert(book->getId(), book);
    }

    file.close();
}

void FileManager::saveAuthors(QMap<quint64, Author *> &authors) {
    QFile file(baseDirectory + "authors.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = authors.keys();
    for (int i = 0; i < keys.size(); i++) {
        Author *author = authors.value(keys.at(i));
        if (author == nullptr) {
            continue;
        }
        out << author->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadAuthors(QMap<quint64, Author *> &authors) {
    QFile file(baseDirectory + "authors.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Author *author = new Author();
        author->deserialize(line);
        BaseEntity::registerExistingId(author->getId());
        if (authors.contains(author->getId())) {
            delete authors.value(author->getId());
        }
        authors.insert(author->getId(), author);
    }

    file.close();
}

void FileManager::saveLibraries(QMap<quint64, Library *> &libraries) {
    QFile file(baseDirectory + "libraries.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = libraries.keys();
    for (int i = 0; i < keys.size(); i++) {
        Library *library = libraries.value(keys.at(i));
        if (library == nullptr) {
            continue;
        }
        out << library->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadLibraries(QMap<quint64, Library *> &libraries) {
    QFile file(baseDirectory + "libraries.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Library *library = new Library();
        library->deserialize(line);
        BaseEntity::registerExistingId(library->getId());
        if (libraries.contains(library->getId())) {
            delete libraries.value(library->getId());
        }
        libraries.insert(library->getId(), library);
    }

    file.close();
}

void FileManager::saveShelves(QMap<quint64, Shelf *> &shelves) {
    QFile file(baseDirectory + "shelves.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = shelves.keys();
    for (int i = 0; i < keys.size(); i++) {
        Shelf *shelf = shelves.value(keys.at(i));
        if (shelf == nullptr) {
            continue;
        }
        out << shelf->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadShelves(QMap<quint64, Shelf *> &shelves) {
    QFile file(baseDirectory + "shelves.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Shelf *shelf = new Shelf();
        shelf->deserialize(line);
        BaseEntity::registerExistingId(shelf->getId());
        if (shelves.contains(shelf->getId())) {
            delete shelves.value(shelf->getId());
        }
        shelves.insert(shelf->getId(), shelf);
    }

    file.close();
}

void FileManager::saveReviews(QMap<quint64, Review *> &reviews) {
    QFile file(baseDirectory + "reviews.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = reviews.keys();
    for (int i = 0; i < keys.size(); i++) {
        Review *review = reviews.value(keys.at(i));
        if (review == nullptr) {
            continue;
        }
        out << review->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadReviews(QMap<quint64, Review *> &reviews) {
    QFile file(baseDirectory + "reviews.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Review *review = new Review();
        review->deserialize(line);
        BaseEntity::registerExistingId(review->getId());
        if (reviews.contains(review->getId())) {
            delete reviews.value(review->getId());
        }
        reviews.insert(review->getId(), review);
    }

    file.close();
}

void FileManager::saveCarts(QMap<quint64, Cart *> &carts) {
    QFile file(baseDirectory + "carts.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = carts.keys();
    for (int i = 0; i < keys.size(); i++) {
        Cart *cart = carts.value(keys.at(i));
        if (cart == nullptr) {
            continue;
        }
        out << cart->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadCarts(QMap<quint64, Cart *> &carts) {
    QFile file(baseDirectory + "carts.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Cart *cart = new Cart();
        cart->deserialize(line);
        BaseEntity::registerExistingId(cart->getId());
        if (carts.contains(cart->getId())) {
            delete carts.value(cart->getId());
        }
        carts.insert(cart->getId(), cart);
    }

    file.close();
}

void FileManager::saveNotifications(QMap<quint64, Notification *> &notifications) {
    QFile file(baseDirectory + "notifications.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = notifications.keys();
    for (int i = 0; i < keys.size(); i++) {
        Notification *notification = notifications.value(keys.at(i));
        if (notification == nullptr) {
            continue;
        }
        out << notification->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadNotifications(QMap<quint64, Notification *> &notifications) {
    QFile file(baseDirectory + "notifications.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Notification *notification = new Notification();
        notification->deserialize(line);
        BaseEntity::registerExistingId(notification->getId());
        if (notifications.contains(notification->getId())) {
            delete notifications.value(notification->getId());
        }
        notifications.insert(notification->getId(), notification);
    }

    file.close();
}

void FileManager::saveWallets(QMap<quint64, Wallet *> &wallets) {
    QFile file(baseDirectory + "wallets.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = wallets.keys();
    for (int i = 0; i < keys.size(); i++) {
        Wallet *wallet = wallets.value(keys.at(i));
        if (wallet == nullptr) {
            continue;
        }
        out << wallet->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadWallets(QMap<quint64, Wallet *> &wallets) {
    QFile file(baseDirectory + "wallets.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Wallet *wallet = new Wallet();
        wallet->deserialize(line);
        BaseEntity::registerExistingId(wallet->getId());
        if (wallets.contains(wallet->getId())) {
            delete wallets.value(wallet->getId());
        }
        wallets.insert(wallet->getId(), wallet);
    }

    file.close();
}

void FileManager::saveTransactions(QMap<quint64, Transaction *> &transactions) {
    QFile file(baseDirectory + "transactions.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = transactions.keys();
    for (int i = 0; i < keys.size(); i++) {
        Transaction *transaction = transactions.value(keys.at(i));
        if (transaction == nullptr) {
            continue;
        }
        out << transaction->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadTransactions(QMap<quint64, Transaction *> &transactions) {
    QFile file(baseDirectory + "transactions.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Transaction *transaction = new Transaction();
        transaction->deserialize(line);
        BaseEntity::registerExistingId(transaction->getId());
        if (transactions.contains(transaction->getId())) {
            delete transactions.value(transaction->getId());
        }
        transactions.insert(transaction->getId(), transaction);
    }

    file.close();
}

void FileManager::savePurchases(QMap<quint64, Purchase *> &purchases) {
    QFile file(baseDirectory + "purchases.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    QList<quint64> keys = purchases.keys();
    for (int i = 0; i < keys.size(); i++) {
        Purchase *purchase = purchases.value(keys.at(i));
        if (purchase == nullptr) {
            continue;
        }
        out << purchase->serialize() << "\n";
    }

    file.close();
}

void FileManager::loadPurchases(QMap<quint64, Purchase *> &purchases) {
    QFile file(baseDirectory + "purchases.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().length() == 0) {
            continue;
        }

        Purchase *purchase = new Purchase();
        purchase->deserialize(line);
        BaseEntity::registerExistingId(purchase->getId());
        if (purchases.contains(purchase->getId())) {
            delete purchases.value(purchase->getId());
        }
        purchases.insert(purchase->getId(), purchase);
    }

    file.close();
}

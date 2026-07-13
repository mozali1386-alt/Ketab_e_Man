//in the name of ALLAH
//YA MAHDI

#include "FileManager.h"
#include "../ServerCore.h"
#include "../../shared/NormalUser.h"
#include "../../shared/Publisher.h"
#include "../../shared/Admin.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>

FileManager::FileManager() {
#ifdef PROJECT_ROOT_DIR
    baseDirectory = QString(PROJECT_ROOT_DIR) + "/assets/data/";
#else
    baseDirectory = "assets/data/";
#endif
}

void FileManager::ensureBaseDirectoryExists() {
    QDir dir;
    dir.mkpath(baseDirectory);
}

bool FileManager::saveAllData(ServerCore *server) {
    if (server == nullptr) {
        return false;
    }

    ensureBaseDirectoryExists();

    saveUsers(server->getUsersMap());
    saveBooks(server->getBooksMap());
    saveAuthors(server->getAuthorsMap());
    saveLibraries(server->getLibrariesMap());
    saveShelves(server->getShelvesMap());
    saveReviews(server->getReviewsMap());
    saveCarts(server->getCartsMap());
    saveNotifications(server->getNotificationsMap());
    saveWallets(server->getWalletsMap());
    saveTransactions(server->getTransactionsMap());
    savePurchases(server->getPurchasesMap());

    return true;
}

bool FileManager::loadAllData(ServerCore *server) {
    if (server == nullptr) {
        return false;
    }

    ensureBaseDirectoryExists();

    loadUsers(server->getUsersMap());
    loadBooks(server->getBooksMap());
    loadAuthors(server->getAuthorsMap());
    loadLibraries(server->getLibrariesMap());
    loadShelves(server->getShelvesMap());
    loadReviews(server->getReviewsMap());
    loadCarts(server->getCartsMap());
    loadNotifications(server->getNotificationsMap());
    loadWallets(server->getWalletsMap());
    loadTransactions(server->getTransactionsMap());
    loadPurchases(server->getPurchasesMap());

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
        purchases.insert(purchase->getId(), purchase);
    }

    file.close();
}

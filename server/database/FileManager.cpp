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


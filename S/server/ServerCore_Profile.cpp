//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Wallet.h"
#include "../shared/Purchase.h"
#include "../shared/Transaction.h"

void ServerCore::handleGetDataUserProfileRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    User *user = data.getUsersMap().value(userId, nullptr);
    if (user == nullptr) {
        handler->sendResponse(Command::FAIL, {"User not found"});
        return;
    }

    QString genresList = "";
    if (user->getRole() == Role::USER) {
        NormalUser *normalUser = static_cast<NormalUser *>(user);
        QStringList genreParts;
        for (Genre genre: normalUser->getFavoriteGenres()) {
            genreParts.append(genreToString(genre));
        }
        genresList = genreParts.join(",");
    }

    qint64 walletBalance = 0;
    Wallet *wallet = data.getWalletsMap().value(user->getWalletId(), nullptr);
    if (wallet != nullptr) {
        walletBalance = wallet->getBalance();
    }

    handler->sendResponse(Command::USER_PROFILE_INFO, {
                              user->getFullName(),
                              user->getUsername(),
                              user->getEmail(),
                              genresList,
                              QString::number(walletBalance)
                          });
}

void ServerCore::handleGetHistoryIdsRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QSet<quint64> bookIds;
    QMap<quint64, Purchase *> &purchases = data.getPurchasesMap();
    for (auto it = purchases.constBegin(); it != purchases.constEnd(); ++it) {
        Purchase *purchase = it.value();
        if (purchase->getBuyerId() != userId) {
            continue;
        }
        QSet<quint64> purchasedInThis = purchase->getBookIds();
        for (quint64 bookId: purchasedInThis) {
            bookIds.insert(bookId);
        }
    }

    if (bookIds.isEmpty()) {
        handler->sendResponse(Command::HISTORY_IDS, {"0"});
        return;
    }

    QStringList idsList;
    for (quint64 bookId: bookIds) {
        idsList.append(QString::number(bookId));
    }

    handler->sendResponse(Command::HISTORY_IDS, {QString::number(bookIds.size()), idsList.join(",")});
}

void ServerCore::handleGetHistoryBookRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }


    Purchase *foundPurchase = nullptr;
    QMap<quint64, Purchase *> &purchases = data.getPurchasesMap();
    for (auto it = purchases.constBegin(); it != purchases.constEnd(); ++it) {
        Purchase *purchase = it.value();
        if (purchase->getBuyerId() != userId || !purchase->getBookIds().contains(bookId)) {
            continue;
        }
        if (foundPurchase == nullptr || purchase->getPurchaseDate() > foundPurchase->getPurchaseDate()) {
            foundPurchase = purchase;
        }
    }

    if (foundPurchase == nullptr) {
        handler->sendResponse(Command::FAIL, {"Purchase not found"});
        return;
    }

    QString authorName = getAuthorName(book->getAuthorId());


    qint64 buyPrice = book->getFinalPrice();
    qint64 bestDiffSeconds = -1;

    QMap<quint64, Transaction *> &transactions = data.getTransactionsMap();
    for (auto it = transactions.constBegin(); it != transactions.constEnd(); ++it) {
        Transaction *transaction = it.value();
        if (transaction->getType() != TransactionType::SALE_INCOME) {
            continue;
        }

        QPair<quint64, qint64> priceInfo = transaction->getBookPriceAtPurchase();
        if (priceInfo.first != bookId) {
            continue;
        }

        qint64 diffSeconds = qAbs(transaction->getCreatedAt().secsTo(foundPurchase->getPurchaseDate()));
        if (bestDiffSeconds == -1 || diffSeconds < bestDiffSeconds) {
            bestDiffSeconds = diffSeconds;
            buyPrice = priceInfo.second;
        }
    }

    handler->sendResponse(Command::HISTORY_BOOK_INFO, {
                              QString::number(bookId),
                              book->getTitle(),
                              authorName,
                              QString::number(buyPrice),
                              foundPurchase->getPurchaseDate().toString("yyyy/MM/dd")
                          });
}

void ServerCore::handleUpdateBalanceRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    bool ok = false;
    qint64 newBalance = fields.at(0).toLongLong(&ok);
    if (!ok || newBalance < 0) {
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    User *user = data.getUsersMap().value(userId, nullptr);
    if (user == nullptr) {
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    Wallet *wallet = data.getWalletsMap().value(user->getWalletId(), nullptr);
    if (wallet == nullptr) {
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    qint64 currentBalance = wallet->getBalance();
    if (newBalance > currentBalance) {
        wallet->deposit(newBalance - currentBalance);
    } else if (newBalance < currentBalance) {
        wallet->withdraw(currentBalance - newBalance);
    }

    handler->sendResponse(Command::SUCCESS, {});
}

void ServerCore::handleUpdateProfileRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 3) {
        handler->sendResponse(Command::UPDATE_PROFILE_RESULT, {"DUPLICATE_EMAIL"});
        return;
    }

    User *user = data.getUsersMap().value(userId, nullptr);
    if (user == nullptr) {
        handler->sendResponse(Command::UPDATE_PROFILE_RESULT, {"DUPLICATE_EMAIL"});
        return;
    }

    QString newName = fields.at(0);
    QString newEmail = fields.at(1);
    QString genresString = fields.at(2);

    if (newEmail != user->getEmail()) {
        User *existing = data.findUserByEmail(newEmail);
        if (existing != nullptr && existing->getId() != userId) {
            handler->sendResponse(Command::UPDATE_PROFILE_RESULT, {"DUPLICATE_EMAIL"});
            return;
        }
        data.unregisterEmail(user->getEmail());
        data.registerEmail(newEmail, userId);
    }

    user->setFullName(newName);
    user->setEmail(newEmail);

    if (user->getRole() == Role::USER) {
        NormalUser *normalUser = static_cast<NormalUser *>(user);
        QSet<Genre> currentGenres = normalUser->getFavoriteGenres();
        for (Genre genre: currentGenres) {
            normalUser->removeFavoriteGenre(genre);
        }
        QStringList parts = genresString.split(",");
        for (int i = 0; i < parts.size(); i++) {
            Genre genre;
            if (stringToGenre(parts.at(i), genre)) {
                normalUser->addFavoriteGenre(genre);
            }
        }
    }

    handler->sendResponse(Command::UPDATE_PROFILE_RESULT, {"SUCCESS"});
}

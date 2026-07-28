//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Review.h"
#include "../shared/Publisher.h"
#include "../shared/Cart.h"
#include "../shared/Shelf.h"
#include "../shared/Purchase.h"
#include "../shared/Wallet.h"
#include "../shared/Transaction.h"
#include "../shared/Notification.h"
#include "../shared/Author.h"
#include <QVector>

void ServerCore::handleSearchUsersRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 3) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    QString nameFilter = fields.at(0);
    if (nameFilter == "EMPTY") {
        nameFilter = "";
    }
    QString roleFilter = fields.at(1);
    QString statusFilter = fields.at(2);

    QStringList matchedIds;
    QMap<quint64, User *> &users = data.getUsersMap();
    for (auto it = users.constBegin(); it != users.constEnd(); ++it) {
        User *user = it.value();

        if (user->getRole() == Role::ADMIN) {
            continue;
        }

        if (roleFilter == "PUBLISHER" && user->getRole() != Role::PUBLISHER) {
            continue;
        }
        if (roleFilter == "NORMAL" && user->getRole() != Role::USER) {
            continue;
        }

        if (statusFilter == "BLOCK" && !user->getIsBlocked()) {
            continue;
        }
        if (statusFilter == "UNBLOCK" && user->getIsBlocked()) {
            continue;
        }

        if (!nameFilter.isEmpty() && !user->getFullName().contains(nameFilter, Qt::CaseInsensitive)) {
            continue;
        }

        matchedIds.append(QString::number(user->getId()));
    }

    if (matchedIds.isEmpty()) {
        handler->sendResponse(Command::USERS_LIST_RESULT, {"0"});
        return;
    }

    handler->sendResponse(Command::USERS_LIST_RESULT, {QString::number(matchedIds.size()), matchedIds.join(",")});
}

void ServerCore::handleGetUserInfoRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 targetId = fields.at(0).toULongLong();
    User *target = data.getUsersMap().value(targetId, nullptr);
    if (target == nullptr || target->getRole() == Role::ADMIN) {
        handler->sendResponse(Command::FAIL, {"User not found"});
        return;
    }

    QString roleString = (target->getRole() == Role::PUBLISHER) ? "PUBLISHER" : "NORMAL";
    QString statusString = target->getIsBlocked() ? "BLOCK" : "UNBLOCK";

    handler->sendResponse(Command::USER_INFO_RESULT, {
                              QString::number(targetId),
                              target->getFullName(),
                              target->getUsername(),
                              roleString,
                              target->getEmail(),
                              target->getCreatedAt().toString("yyyy/MM/dd"),
                              statusString
                          });
}

void ServerCore::handleDeleteUserRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::DELETE_USER_RESULT, {"0", "FAILED"});
        return;
    }

    quint64 targetId = fields.at(0).toULongLong();
    User *target = data.getUsersMap().value(targetId, nullptr);
    if (target == nullptr || target->getRole() == Role::ADMIN) {
        handler->sendResponse(Command::DELETE_USER_RESULT, {QString::number(targetId), "FAILED"});
        return;
    }

    ClientHandler *onlineHandler = findClientHandlerByUserId(targetId);
    if (onlineHandler != nullptr) {
        onlineHandler->setUserId(0);
        loggedInClients.remove(targetId);
        onlineHandler->sendResponse(Command::FAIL, {"Your account was deleted"});
    }


    if (target->getRole() == Role::PUBLISHER) {
        Publisher *publisher = static_cast<Publisher *>(target);
        QSet<quint64> bookIds = publisher->getMyBookIds();
        for (quint64 bookId: bookIds) {
            cascadeDeleteBook(bookId);
        }
    } else if (target->getRole() == Role::USER) {
        NormalUser *normalUser = static_cast<NormalUser *>(target);

        cascadeDeleteReviewsByAuthor(targetId);

        Cart *cart = data.findCartByOwner(targetId);
        if (cart != nullptr) {
            data.getCartsMap().remove(cart->getId());
            delete cart;
        }

        Library *library = data.getLibrariesMap().value(normalUser->getLibraryId(), nullptr);
        if (library != nullptr) {
            QSet<quint64> shelfIds = library->getShelves();
            for (quint64 shelfId: shelfIds) {
                Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
                if (shelf != nullptr) {
                    data.getShelvesMap().remove(shelfId);
                    delete shelf;
                }
            }
            data.getLibrariesMap().remove(library->getId());
            delete library;
        }

        QMap<quint64, Purchase *> &purchases = data.getPurchasesMap();
        QVector<quint64> purchaseIdsToRemove;
        for (auto it = purchases.constBegin(); it != purchases.constEnd(); ++it) {
            if (it.value()->getBuyerId() == targetId) {
                purchaseIdsToRemove.append(it.key());
            }
        }
        for (quint64 purchaseId: purchaseIdsToRemove) {
            Purchase *purchase = purchases.value(purchaseId, nullptr);
            purchases.remove(purchaseId);
            delete purchase;
        }
    }

    cascadeDeleteWalletAndTransactions(target->getWalletId());
    cascadeDeleteNotificationsFor(targetId);

    data.getUsersMap().remove(targetId);
    delete target;

    handler->sendResponse(Command::DELETE_USER_RESULT, {QString::number(targetId), "SUCCESS"});
}

void ServerCore::handleBlockUserRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::BLOCK_USER_RESULT, {"0", "FAILED"});
        return;
    }

    quint64 targetId = fields.at(0).toULongLong();
    User *target = data.getUsersMap().value(targetId, nullptr);
    if (target == nullptr || target->getRole() == Role::ADMIN) {
        handler->sendResponse(Command::BLOCK_USER_RESULT, {QString::number(targetId), "FAILED"});
        return;
    }

    target->setIsBlocked(true);


    ClientHandler *onlineHandler = findClientHandlerByUserId(targetId);
    if (onlineHandler != nullptr) {
        onlineHandler->setUserId(0);
        loggedInClients.remove(targetId);
        onlineHandler->sendResponse(Command::FAIL, {"Your account has been blocked"});
    }

    handler->sendResponse(Command::BLOCK_USER_RESULT, {QString::number(targetId), "SUCCESS"});
}

void ServerCore::handleUnblockUserRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::UNBLOCK_USER_RESULT, {"0", "FAILED"});
        return;
    }

    quint64 targetId = fields.at(0).toULongLong();
    User *target = data.getUsersMap().value(targetId, nullptr);
    if (target == nullptr || target->getRole() == Role::ADMIN) {
        handler->sendResponse(Command::UNBLOCK_USER_RESULT, {QString::number(targetId), "FAILED"});
        return;
    }

    target->setIsBlocked(false);

    handler->sendResponse(Command::UNBLOCK_USER_RESULT, {QString::number(targetId), "SUCCESS"});
}

void ServerCore::handleDeleteCommentAdminRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::DELETE_COMMENT_ADMIN_RESULT, {"0", "FAILED"});
        return;
    }

    quint64 reviewId = 0;
    if (!parseCommentId(fields.at(0), reviewId)) {
        handler->sendResponse(Command::DELETE_COMMENT_ADMIN_RESULT, {fields.at(0), "FAILED"});
        return;
    }

    Review *review = data.getReviewsMap().value(reviewId, nullptr);
    if (review == nullptr) {
        handler->sendResponse(Command::DELETE_COMMENT_ADMIN_RESULT, {fields.at(0), "FAILED"});
        return;
    }

    Book *book = data.getBooksMap().value(review->getBookId(), nullptr);
    if (book != nullptr) {
        book->removeReview(reviewId);
    }

    data.getReviewsMap().remove(reviewId);
    delete review;

    handler->sendResponse(Command::DELETE_COMMENT_ADMIN_RESULT, {fields.at(0), "SUCCESS"});
}

void ServerCore::cascadeDeleteBook(quint64 bookId) {
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        return;
    }


    QSet<quint64> reviewIds = book->getReviewIds();
    for (quint64 reviewId: reviewIds) {
        Review *review = data.getReviewsMap().value(reviewId, nullptr);
        if (review != nullptr) {
            data.getReviewsMap().remove(reviewId);
            delete review;
        }
    }


    QMap<quint64, Cart *> &carts = data.getCartsMap();
    for (auto it = carts.constBegin(); it != carts.constEnd(); ++it) {
        it.value()->removeItem(bookId);
    }


    QMap<quint64, Shelf *> &shelves = data.getShelvesMap();
    for (auto it = shelves.constBegin(); it != shelves.constEnd(); ++it) {
        it.value()->removeBook(bookId);
    }


    QMap<quint64, Library *> &libraries = data.getLibrariesMap();
    for (auto it = libraries.constBegin(); it != libraries.constEnd(); ++it) {
        it.value()->removeFromSavedBooks(bookId);
        it.value()->removeFromPurchasedBooks(bookId);
    }


    Author *author = data.getAuthorsMap().value(book->getAuthorId(), nullptr);
    if (author != nullptr) {
        author->removeBook(bookId);
    }

    data.getBooksMap().remove(bookId);
    delete book;
}

void ServerCore::cascadeDeleteReviewsByAuthor(quint64 userId) {
    QMap<quint64, Review *> &reviews = data.getReviewsMap();

    QVector<quint64> reviewIdsToRemove;
    for (auto it = reviews.constBegin(); it != reviews.constEnd(); ++it) {
        if (it.value()->getUserId() == userId) {
            reviewIdsToRemove.append(it.key());
        }
    }

    for (quint64 reviewId: reviewIdsToRemove) {
        Review *review = reviews.value(reviewId, nullptr);
        if (review == nullptr) {
            continue;
        }

        Book *book = data.getBooksMap().value(review->getBookId(), nullptr);
        if (book != nullptr) {
            book->removeReview(reviewId);
        }

        reviews.remove(reviewId);
        delete review;
    }
}

void ServerCore::cascadeDeleteWalletAndTransactions(quint64 walletId) {
    Wallet *wallet = data.getWalletsMap().value(walletId, nullptr);
    if (wallet == nullptr) {
        return;
    }

    QVector<quint64> transactionIds = wallet->getTransactionIds();
    QMap<quint64, Transaction *> &transactions = data.getTransactionsMap();
    for (quint64 transactionId: transactionIds) {
        Transaction *transaction = transactions.value(transactionId, nullptr);
        if (transaction != nullptr) {
            transactions.remove(transactionId);
            delete transaction;
        }
    }

    data.getWalletsMap().remove(walletId);
    delete wallet;
}

void ServerCore::cascadeDeleteNotificationsFor(quint64 userId) {
    QMap<quint64, Notification *> &notifications = data.getNotificationsMap();

    QVector<quint64> notificationIdsToRemove;
    for (auto it = notifications.constBegin(); it != notifications.constEnd(); ++it) {
        if (it.value()->getRecipientId() == userId) {
            notificationIdsToRemove.append(it.key());
        }
    }

    for (quint64 notificationId: notificationIdsToRemove) {
        Notification *notification = notifications.value(notificationId, nullptr);
        if (notification != nullptr) {
            notifications.remove(notificationId);
            delete notification;
        }
    }
}

void ServerCore::handleGetAllCommentsAdminRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    QMap<quint64, Review *> &reviews = data.getReviewsMap();
    if (reviews.isEmpty()) {
        handler->sendResponse(Command::ALL_COMMENTS_ADMIN_RESULT, {"0"});
        return;
    }

    QStringList idsList;
    for (auto it = reviews.constBegin(); it != reviews.constEnd(); ++it) {
        idsList.append(formatCommentId(it.key()));
    }

    handler->sendResponse(Command::ALL_COMMENTS_ADMIN_RESULT, {QString::number(idsList.size()), idsList.join(",")});
}

void ServerCore::handleGetCommentInfoRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 reviewId = 0;
    if (!parseCommentId(fields.at(0), reviewId)) {
        handler->sendResponse(Command::FAIL, {"Comment not found"});
        return;
    }

    Review *review = data.getReviewsMap().value(reviewId, nullptr);
    if (review == nullptr) {
        handler->sendResponse(Command::FAIL, {"Comment not found"});
        return;
    }

    User *author = data.getUsersMap().value(review->getUserId(), nullptr);
    QString userName = (author != nullptr) ? author->getUsername() : "";

    Book *book = data.getBooksMap().value(review->getBookId(), nullptr);
    QString bookName = (book != nullptr) ? book->getTitle() : "";

    handler->sendResponse(Command::COMMENT_INFO_RESULT, {
                              formatCommentId(reviewId),
                              userName,
                              bookName,
                              review->getText(),
                              review->getCreatedAt().toString("yyyy/MM/dd")
                          });
}

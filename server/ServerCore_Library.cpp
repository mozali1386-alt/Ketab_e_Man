//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Protocol.h"
#include <QStringList>

void ServerCore::handleCreateShelfRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    Library *library = requireUserLibrary(userId, handler);
    if (library == nullptr) {
        return;
    }

    Shelf *shelf = new Shelf();
    shelf->assignNewId();
    shelf->setOwnerId(userId);
    shelf->rename(payload);
    data.getShelvesMap().insert(shelf->getId(), shelf);

    library->addShelf(shelf->getId());

    handler->sendResponse(RES_SUCCESS, QString::number(shelf->getId()));
}

void ServerCore::handleAddBookToShelfRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 2, parts)) {
        return;
    }

    quint64 shelfId = parts.at(0).toULongLong();
    quint64 bookId = parts.at(1).toULongLong();

    Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
    if (shelf == nullptr || shelf->getOwnerId() != userId) {
        handler->sendResponse(RES_FAIL, "Not authorized");
        return;
    }

    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || !book->getIsActive()) {
        handler->sendResponse(RES_FAIL, "Book not found");
        return;
    }

    if (!libraryOwnerHasPurchasedBook(userId, bookId)) {
        handler->sendResponse(RES_FAIL, "You must purchase this book before adding it to a shelf");
        return;
    }

    shelf->addBook(bookId);
    handler->sendResponse(RES_SUCCESS, "Book added to shelf");
}

void ServerCore::handleSaveBookRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    Library *library = requireUserLibrary(userId, handler);
    if (library == nullptr) {
        return;
    }

    library->addToSavedBooks(payload.toULongLong());
    handler->sendResponse(RES_SUCCESS, "Book saved");
}

void ServerCore::handleAddReviewRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 3, parts)) {
        return;
    }

    quint64 bookId = parts.at(0).toULongLong();

    User *user = data.getUsersMap().value(userId, nullptr);
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (user == nullptr || book == nullptr) {
        handler->sendResponse(RES_FAIL, "Invalid data");
        return;
    }

    if (!libraryOwnerHasPurchasedBook(userId, bookId)) {
        handler->sendResponse(RES_FAIL, "You must purchase this book before reviewing it");
        return;
    }

    Review *review = new Review();
    review->assignNewId();
    review->setUserId(userId);
    review->setBookId(bookId);
    review->editStars(parts.at(1).toInt());
    review->editText(parts.at(2));
    data.getReviewsMap().insert(review->getId(), review);

    user->addReview(review->getId());
    book->addReview(review->getId());

    handler->sendResponse(RES_SUCCESS, QString::number(review->getId()));
}

void ServerCore::handleEditReviewRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 3, parts)) {
        return;
    }

    Review *review = data.getReviewsMap().value(parts.at(0).toULongLong(), nullptr);
    if (review == nullptr) {
        handler->sendResponse(RES_FAIL, "Review not found");
        return;
    }

    if (review->getUserId() != userId) {
        handler->sendResponse(RES_FAIL, "Not authorized");
        return;
    }

    review->editStars(parts.at(1).toInt());
    review->editText(parts.at(2));

    broadcastReviewUpdated(review->getBookId());
    handler->sendResponse(RES_SUCCESS, "Review updated");
}

void ServerCore::handleDeleteReviewRequest(ClientHandler *handler, const QString &payload) {
    quint64 adminId = requireAuthentication(handler);
    if (adminId == 0) {
        return;
    }

    Admin *admin = requireAdmin(adminId, handler);
    if (admin == nullptr) {
        return;
    }

    quint64 reviewId = payload.toULongLong();
    Review *review = data.getReviewsMap().value(reviewId, nullptr);
    if (review == nullptr) {
        handler->sendResponse(RES_FAIL, "Review not found");
        return;
    }

    User *reviewOwner = data.getUsersMap().value(review->getUserId(), nullptr);
    if (reviewOwner != nullptr) {
        reviewOwner->removeReview(reviewId);
    }

    Book *book = data.getBooksMap().value(review->getBookId(), nullptr);
    if (book != nullptr) {
        book->removeReview(reviewId);
    }

    data.getReviewsMap().remove(reviewId);
    delete review;

    handler->sendResponse(RES_SUCCESS, "Review deleted");
}

void ServerCore::broadcastReviewUpdated(quint64 bookId) {
    dataMutex.lock();
    QSet<ClientHandler *> clientsCopy = connectedClients;
    dataMutex.unlock();

    QString payload = QString::number(bookId);
    for (ClientHandler *handler: clientsCopy) {
        handler->sendResponse(NOTIFY_REVIEW_UPDATED, payload);
    }
}

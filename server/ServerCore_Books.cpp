//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Protocol.h"
#include "../shared/Publisher.h"
#include <QStringList>

void ServerCore::handlePublishRequest(ClientHandler *handler, const QString &payload) {
    quint64 publisherId = requireAuthentication(handler);
    if (publisherId == 0) {
        return;
    }

    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 7, parts)) {
        return;
    }

    User *publisherUser = data.getUsersMap().value(publisherId, nullptr);
    if (publisherUser == nullptr || publisherUser->getRole() != Role::PUBLISHER) {
        handler->sendResponse(RES_FAIL, "Not authorized");
        return;
    }
    Publisher *publisher = static_cast<Publisher *>(publisherUser);

    QString authorName = parts.at(1);
    Author *author = nullptr;
    QList<quint64> authorKeys = data.getAuthorsMap().keys();
    for (int i = 0; i < authorKeys.size(); i++) {
        Author *candidate = data.getAuthorsMap().value(authorKeys.at(i));
        if (candidate->getFullName() == authorName) {
            author = candidate;
            break;
        }
    }
    if (author == nullptr) {
        author = new Author();
        author->assignNewId();
        author->setFullName(authorName);
        data.getAuthorsMap().insert(author->getId(), author);
    }

    Genre genre;
    double price = 0.0;
    if (!parseGenre(parts.at(2), genre) || !parseDouble(parts.at(4), price)) {
        handler->sendResponse(RES_FAIL, "Invalid genre or price");
        return;
    }

    Book *book = new Book();
    book->assignNewId();
    book->setTitle(parts.at(0));
    book->setAuthorId(author->getId());
    book->setPublisherId(publisherId);
    book->setGenre(genre);
    book->setDescription(parts.at(3));
    book->setPrice(price);
    book->setCoverImagePath(parts.at(5));
    book->setPdfFilePath(parts.at(6));
    data.getBooksMap().insert(book->getId(), book);

    author->addBook(book->getId());
    publisher->publishBook(book->getId());
    searchEngine.addBookToIndex(book);
    notifyUsersAboutNewBook(book);

    handler->sendResponse(RES_SUCCESS, QString::number(book->getId()));
}

void ServerCore::handleUpdateBookRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 3, parts)) {
        return;
    }

    Book *book = requireOwnedBook(parts.at(0).toULongLong(), userId, handler);
    if (book == nullptr) {
        return;
    }

    double newPrice = 0.0;
    double newDiscount = 0.0;
    if (!parseDouble(parts.at(1), newPrice) || !parseDouble(parts.at(2), newDiscount)) {
        handler->sendResponse(RES_FAIL, "Invalid price or discount");
        return;
    }

    book->setPrice(newPrice);
    book->applyDiscount(newDiscount);

    handler->sendResponse(RES_SUCCESS, "Book updated");
}

void ServerCore::handleDeactivateBookRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 2, parts)) {
        return;
    }

    Book *book = requireOwnedBook(parts.at(0).toULongLong(), userId, handler);
    if (book == nullptr) {
        return;
    }

    if (parts.at(1).toInt() == 1) {
        Publisher *publisher = static_cast<Publisher *>(data.getUsersMap().value(userId));
        if (!publisher->getMyBookIds().contains(book->getId())) {
            handler->sendResponse(RES_FAIL, "This book was removed by an administrator and cannot be reactivated");
            return;
        }
        book->reactivate();
        searchEngine.addBookToIndex(book);
    } else {
        book->deactivate();
        purgeBookFromNonOwnerPlaces(book->getId());
    }

    handler->sendResponse(RES_SUCCESS, "Book state updated");
}

void ServerCore::handleDeleteBookRequest(ClientHandler *handler, const QString &payload) {
    quint64 adminId = requireAuthentication(handler);
    if (adminId == 0) {
        return;
    }

    Admin *admin = requireAdmin(adminId, handler);
    if (admin == nullptr) {
        return;
    }

    quint64 bookId = payload.toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        handler->sendResponse(RES_FAIL, "Book not found");
        return;
    }

    Author *author = data.getAuthorsMap().value(book->getAuthorId(), nullptr);
    if (author != nullptr) {
        author->removeBook(bookId);
    }

    User *publisherUser = data.getUsersMap().value(book->getPublisherId(), nullptr);
    if (publisherUser != nullptr && publisherUser->getRole() == Role::PUBLISHER) {
        static_cast<Publisher *>(publisherUser)->removeBook(bookId);
    }

    deleteAllReviewsForBook(bookId, book);
    book->deactivate();
    purgeBookFromNonOwnerPlaces(bookId);

    handler->sendResponse(RES_SUCCESS, "Book deleted");
    emit logMessageGenerated("Book permanently removed by admin: " + QString::number(bookId));
}

void ServerCore::handleSearchRequest(ClientHandler *handler, const QString &payload) {
    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 2, parts)) {
        return;
    }

    QString type = parts.at(0);
    QString query = parts.at(1);
    QSet<quint64> result;

    if (type == "title") {
        result = searchEngine.searchByTitle(query);
    } else if (type == "author") {
        result = searchEngine.searchByAuthor(query.toULongLong());
    } else if (type == "publisher") {
        result = searchEngine.searchByPublisher(query.toULongLong());
    } else if (type == "genre") {
        Genre genre;
        if (parseGenre(query, genre)) {
            result = searchEngine.filterByGenre(genre);
        }
    }

    QStringList idList;
    for (quint64 bookId: result) {
        idList.append(QString::number(bookId));
    }

    handler->sendResponse(RES_SEARCH_RESULT, idList.join(","));
}

void ServerCore::notifyUsersAboutNewBook(Book *book) {
    QList<quint64> userKeys = data.getUsersMap().keys();
    for (int i = 0; i < userKeys.size(); i++) {
        User *user = data.getUsersMap().value(userKeys.at(i));
        if (user->getRole() != Role::USER) {
            continue;
        }

        NormalUser *normalUser = static_cast<NormalUser *>(user);
        if (normalUser->getFavoriteGenres().contains(book->getGenre())) {
            QString message = "A new book '" + book->getTitle() + "' was added in your favorite genre!";
            sendNotificationToUser(user->getId(), NotificationType::NEW_BOOK_IN_FAVORITE_GENRE, message);
        }
    }
}

bool ServerCore::libraryOwnerHasPurchasedBook(quint64 ownerId, quint64 bookId) {
    QList<quint64> libraryKeys = data.getLibrariesMap().keys();
    for (int i = 0; i < libraryKeys.size(); i++) {
        Library *library = data.getLibrariesMap().value(libraryKeys.at(i));
        if (library->getOwnerId() == ownerId) {
            return library->getPurchasedBooks().contains(bookId);
        }
    }
    return false;
}

void ServerCore::purgeBookFromNonOwnerPlaces(quint64 bookId) {
    searchEngine.removeBookFromIndex(bookId);

    QList<quint64> cartKeys = data.getCartsMap().keys();
    for (int i = 0; i < cartKeys.size(); i++) {
        data.getCartsMap().value(cartKeys.at(i))->removeItem(bookId);
    }

    QList<quint64> libraryKeys = data.getLibrariesMap().keys();
    for (int i = 0; i < libraryKeys.size(); i++) {
        Library *library = data.getLibrariesMap().value(libraryKeys.at(i));
        if (!library->getPurchasedBooks().contains(bookId)) {
            library->removeFromSavedBooks(bookId);
        }
    }

    QList<quint64> shelfKeys = data.getShelvesMap().keys();
    for (int i = 0; i < shelfKeys.size(); i++) {
        Shelf *shelf = data.getShelvesMap().value(shelfKeys.at(i));
        if (!libraryOwnerHasPurchasedBook(shelf->getOwnerId(), bookId)) {
            shelf->removeBook(bookId);
        }
    }
}

void ServerCore::deleteAllReviewsForBook(quint64 bookId, Book *book) {
    QList<quint64> reviewKeys = data.getReviewsMap().keys();
    for (int i = 0; i < reviewKeys.size(); i++) {
        quint64 reviewId = reviewKeys.at(i);
        Review *review = data.getReviewsMap().value(reviewId);
        if (review->getBookId() != bookId) {
            continue;
        }

        User *reviewOwner = data.getUsersMap().value(review->getUserId(), nullptr);
        if (reviewOwner != nullptr) {
            reviewOwner->removeReview(reviewId);
        }
        if (book != nullptr) {
            book->removeReview(reviewId);
        }

        data.getReviewsMap().remove(reviewId);
        delete review;
    }
}

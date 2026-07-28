//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Author.h"
#include "../shared/Publisher.h"
#include "../shared/Review.h"
#include "../shared/Cart.h"
#include <QVector>
#include <algorithm>

void ServerCore::handleGetPublisherBooksRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Publisher *publisher = static_cast<Publisher *>(user);
    QSet<quint64> bookIds = publisher->getMyBookIds();

    QStringList entries;
    for (quint64 bookId: bookIds) {
        Book *book = data.getBooksMap().value(bookId, nullptr);
        if (book == nullptr) {
            continue;
        }
        entries.append(QString::number(bookId) + ":" + book->getTitle());
    }

    handler->sendResponse(Command::PUBLISHER_BOOKS_RESULT, {QString::number(entries.size()), entries.join(",")});
}

void ServerCore::handleGetPubBookDetailsRequest(ClientHandler *handler, const QStringList &fields) {
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

    User *user = data.getUsersMap().value(userId, nullptr);
    bool isOwnerPublisher = (user != nullptr && user->getRole() == Role::PUBLISHER && book->getPublisherId() == userId);
    bool isAdmin = (user != nullptr && user->getRole() == Role::ADMIN);
    if (!isOwnerPublisher && !isAdmin) {
        handler->sendResponse(Command::FAIL, {"Not authorized"});
        return;
    }

    QString authorName = getAuthorName(book->getAuthorId());
    QString imageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());

    handler->sendResponse(Command::PUB_BOOK_DETAILS_RESULT, {
                              QString::number(book->getId()),
                              book->getTitle(),
                              authorName,
                              genreToString(book->getGenre()),
                              QString::number(book->getPrice()),
                              QString::number(book->getDiscountPercent()),
                              book->getDescription(),
                              book->getIsActive() ? "1" : "0",
                              imageBase64
                          });
}

void ServerCore::handleGetBookDetailsRequest(ClientHandler *handler, const QStringList &fields) {
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

    if (!book->getIsActive() && !canViewInactiveBook(handler, book)) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    User *requester = data.getUsersMap().value(userId, nullptr);
    QString username = (requester != nullptr) ? requester->getUsername() : "";

    QString imageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());
    QString authorName = getAuthorName(book->getAuthorId());

    User *publisherUser = data.getUsersMap().value(book->getPublisherId(), nullptr);
    QString publisherName = (publisherUser != nullptr) ? publisherUser->getFullName() : "";

    double average = averageStarsForBook(bookId);

    bool isInCart = false;
    Cart *cart = data.findCartByOwner(userId);
    if (cart != nullptr) {
        isInCart = cart->getBookIds().contains(bookId);
    }

    bool isInSaveBook = false;
    Library *library = getLibraryForUser(userId);
    if (library != nullptr) {
        isInSaveBook = library->getSavedBooks().contains(bookId);
    }

    bool isPurchase = userHasPurchasedBook(userId, bookId);

    handler->sendResponse(Command::BOOK_DETAILS_RESULT, {
                              QString::number(userId),
                              username,
                              imageBase64,
                              book->getTitle(),
                              authorName,
                              publisherName,
                              genreToString(book->getGenre()),
                              QString::number(book->getPrice()),
                              QString::number(book->getDiscountPercent() * book->getPrice()),
                              QString::number(average, 'f', 1),
                              book->getDescription(),
                              isInCart ? "1" : "0",
                              isInSaveBook ? "1" : "0",
                              isPurchase ? "1" : "0"
                          });
}

void ServerCore::handleAddBookMetadataRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 7) {
        handler->sendResponse(Command::ADD_BOOK_RESULT, {"FAIL"});
        return;
    }

    QString bookName = fields.at(0);
    QString authorName = fields.at(1);

    Genre genre;
    if (!stringToGenre(fields.at(2), genre)) {
        handler->sendResponse(Command::ADD_BOOK_RESULT, {"FAIL"});
        return;
    }

    bool priceOk = false;
    qint64 price = fields.at(3).toLongLong(&priceOk);
    bool discountOk = false;
    double discount = fields.at(4).toDouble(&discountOk);
    if (!priceOk || !discountOk) {
        handler->sendResponse(Command::ADD_BOOK_RESULT, {"FAIL"});
        return;
    }

    QString explanation = fields.at(5);
    QString coverImageBase64 = fields.at(6);

    quint64 authorId = findOrCreateAuthorByName(authorName);

    Book *book = new Book();
    book->assignNewId();
    book->setTitle(bookName);
    book->setAuthorId(authorId);
    book->setPublisherId(user->getId());
    book->setGenre(genre);
    book->setDescription(explanation);
    book->setPrice(price);
    book->applyDiscount(discount);

    QString coverPath = saveBase64File(coverImageBase64, "covers", QString::number(book->getId()), "img");
    book->setCoverImagePath(coverPath);

    data.getBooksMap().insert(book->getId(), book);

    Author *author = data.getAuthorsMap().value(authorId, nullptr);
    if (author != nullptr) {
        author->addBook(book->getId());
    }

    Publisher *publisher = static_cast<Publisher *>(user);
    publisher->publishBook(book->getId());


    QMap<quint64, User *> &allUsers = data.getUsersMap();
    for (auto it = allUsers.constBegin(); it != allUsers.constEnd(); ++it) {
        User *candidate = it.value();
        if (candidate->getRole() != Role::USER) {
            continue;
        }
        NormalUser *normalCandidate = static_cast<NormalUser *>(candidate);
        if (normalCandidate->getFavoriteGenres().contains(genre)) {
            pushNotification(candidate->getId(), NotificationType::NEW_BOOK_IN_FAVORITE_GENRE,
                             "کتاب جدیدی در ژانر موردعلاقه‌ی شما منتشر شد: " + bookName);
        }
    }

    handler->sendResponse(Command::ADD_BOOK_RESULT, {"SUCCESS", QString::number(book->getId())});
}

void ServerCore::handleUploadPdfStartRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 2) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    bool totalOk = false;
    int totalChunks = fields.at(1).toInt(&totalOk);

    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPublisherId() != user->getId() || !totalOk || totalChunks <= 0) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    PdfUploadState state;
    state.totalChunks = totalChunks;
    pdfUploads.insert(bookId, state);
}

void ServerCore::handleUploadPdfChunkRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 2) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    QString chunkData = fields.at(1);

    if (!pdfUploads.contains(bookId)) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    pdfUploads[bookId].chunks.append(chunkData);
}

void ServerCore::handleUploadPdfEndRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();

    if (!pdfUploads.contains(bookId)) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    PdfUploadState state = pdfUploads.value(bookId);
    pdfUploads.remove(bookId);

    if (state.chunks.size() != state.totalChunks) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPublisherId() != user->getId()) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    QString fullBase64 = state.chunks.join("");
    QString pdfPath = saveBase64File(fullBase64, "pdfs", QString::number(bookId), "pdf");
    if (pdfPath.isEmpty()) {
        handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"FAIL"});
        return;
    }

    book->setPdfFilePath(pdfPath);

    handler->sendResponse(Command::UPLOAD_PDF_RESULT, {"SUCCESS"});
}

void ServerCore::handleEditBookRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 8) {
        handler->sendResponse(Command::EDIT_BOOK_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPublisherId() != user->getId()) {
        handler->sendResponse(Command::EDIT_BOOK_RESULT, {"FAIL"});
        return;
    }

    QString bookName = fields.at(1);
    QString authorName = fields.at(2);

    Genre genre;
    if (!stringToGenre(fields.at(3), genre)) {
        handler->sendResponse(Command::EDIT_BOOK_RESULT, {"FAIL"});
        return;
    }

    bool priceOk = false;
    qint64 price = fields.at(4).toLongLong(&priceOk);
    bool discountOk = false;
    double discount = fields.at(5).toDouble(&discountOk);
    if (!priceOk || !discountOk) {
        handler->sendResponse(Command::EDIT_BOOK_RESULT, {"FAIL"});
        return;
    }

    QString explanation = fields.at(6);
    QString coverImageBase64 = fields.at(7);

    quint64 oldAuthorId = book->getAuthorId();
    quint64 newAuthorId = findOrCreateAuthorByName(authorName);
    if (newAuthorId != oldAuthorId) {
        Author *oldAuthor = data.getAuthorsMap().value(oldAuthorId, nullptr);
        if (oldAuthor != nullptr) {
            oldAuthor->removeBook(bookId);
        }
        Author *newAuthor = data.getAuthorsMap().value(newAuthorId, nullptr);
        if (newAuthor != nullptr) {
            newAuthor->addBook(bookId);
        }
        book->setAuthorId(newAuthorId);
    }

    book->setTitle(bookName);
    book->setGenre(genre);
    book->setPrice(price);
    book->applyDiscount(discount);
    book->setDescription(explanation);


    if (coverImageBase64 != "EMPTY") {
        QString coverPath = saveBase64File(coverImageBase64, "covers", QString::number(bookId), "img");
        if (!coverPath.isEmpty()) {
            book->setCoverImagePath(coverPath);
        }
    }

    handler->sendResponse(Command::EDIT_BOOK_RESULT, {"SUCCESS"});
}

void ServerCore::handleToggleBookStatusRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 2) {
        handler->sendResponse(Command::TOGGLE_STATUS_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    QString action = fields.at(1);

    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPublisherId() != user->getId()) {
        handler->sendResponse(Command::TOGGLE_STATUS_RESULT, {"FAIL"});
        return;
    }


    if (book->getIsDeletedByAdmin()) {
        handler->sendResponse(Command::TOGGLE_STATUS_RESULT, {"FAIL"});
        return;
    }

    if (action == "ENABLE") {
        book->reactivate();
    } else {
        book->deactivate();
    }

    handler->sendResponse(Command::TOGGLE_STATUS_RESULT, {"SUCCESS"});
}

void ServerCore::handleDeleteBookRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::ADMIN);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        handler->sendResponse(Command::DELETE_BOOK_RESULT, {QString::number(bookId), "FAILED"});
        return;
    }


    book->markDeletedByAdmin();

    QSet<quint64> reviewIds = book->getReviewIds();
    for (quint64 reviewId: reviewIds) {
        Review *review = data.getReviewsMap().value(reviewId, nullptr);
        if (review != nullptr) {
            data.getReviewsMap().remove(reviewId);
            delete review;
        }
        book->removeReview(reviewId);
    }


    handler->sendResponse(Command::DELETE_BOOK_RESULT, {QString::number(bookId), "SUCCESS"});
}

void ServerCore::handleSearchStorepageRequest(ClientHandler *handler, const QStringList &fields) {
    if (fields.size() < 5) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    QString bookFilter = fields.at(0);
    QString authorFilter = fields.at(1);
    QString publisherFilter = fields.at(2);
    QString genreFilter = fields.at(3);
    QString display = fields.at(4);

    if (bookFilter == "EMPTY") bookFilter = "";
    if (authorFilter == "EMPTY") authorFilter = "";
    if (publisherFilter == "EMPTY") publisherFilter = "";
    if (genreFilter == "EMPTY") genreFilter = "";
    if (display == "EMPTY") display = "ALL";

    Genre parsedGenre;
    bool hasGenreFilter = !genreFilter.isEmpty() && stringToGenre(genreFilter, parsedGenre);

    QMap<quint64, Book *> &books = data.getBooksMap();

    QSet<quint64> baseSet;
    for (auto it = books.constBegin(); it != books.constEnd(); ++it) {
        Book *book = it.value();
        if (!book->getIsActive()) {
            continue;
        }
        if (!bookFilter.isEmpty() && !book->getTitle().contains(bookFilter, Qt::CaseInsensitive)) {
            continue;
        }
        if (!authorFilter.isEmpty() && QString::number(book->getAuthorId()) != authorFilter) {
            continue;
        }
        if (!publisherFilter.isEmpty() && QString::number(book->getPublisherId()) != publisherFilter) {
            continue;
        }
        if (hasGenreFilter && book->getGenre() != parsedGenre) {
            continue;
        }
        baseSet.insert(book->getId());
    }

    const int maxResults = 20;

    if (display == "RECOMMENDED") {
        quint64 userId = requireAuthentication(handler);
        if (userId == 0) {
            return;
        }

        User *user = data.getUsersMap().value(userId, nullptr);
        if (user == nullptr || user->getRole() != Role::USER) {
            handler->sendResponse(Command::FAIL, {"Not authenticated"});
            return;
        }

        NormalUser *normalUser = static_cast<NormalUser *>(user);
        QSet<Genre> favoriteGenres = normalUser->getFavoriteGenres();

        QSet<quint64> recommendedSet;
        for (auto it = books.constBegin(); it != books.constEnd(); ++it) {
            Book *book = it.value();
            if (favoriteGenres.contains(book->getGenre())) {
                recommendedSet.insert(book->getId());
            }
        }

        baseSet.intersect(recommendedSet);

        QStringList idsList;
        for (quint64 id: baseSet) {
            if (idsList.size() >= maxResults) {
                break;
            }
            idsList.append(QString::number(id));
        }
        handler->sendResponse(Command::SEARCH_RESULT, {idsList.join(",")});
        return;
    }

    if (display == "FREE") {
        QStringList idsList;
        for (quint64 id: baseSet) {
            if (idsList.size() >= maxResults) {
                break;
            }
            Book *book = books.value(id, nullptr);
            if (book != nullptr && book->getFinalPrice() == 0) {
                idsList.append(QString::number(id));
            }
        }
        handler->sendResponse(Command::SEARCH_RESULT, {idsList.join(",")});
        return;
    }

    if (display == "NEW" || display == "BESTSELLER" || display == "POPULAR") {
        QVector<quint64> sortedIds;
        for (quint64 id: baseSet) {
            sortedIds.append(id);
        }

        if (display == "NEW") {
            std::sort(sortedIds.begin(), sortedIds.end(), [&](quint64 a, quint64 b) {
                return books.value(a)->getCreatedAt() > books.value(b)->getCreatedAt();
            });
        } else if (display == "BESTSELLER") {
            std::sort(sortedIds.begin(), sortedIds.end(), [&](quint64 a, quint64 b) {
                return books.value(a)->getSalesCount() > books.value(b)->getSalesCount();
            });
        } else {
            std::sort(sortedIds.begin(), sortedIds.end(), [&](quint64 a, quint64 b) {
                return averageStarsForBook(a) > averageStarsForBook(b);
            });
        }

        QStringList idsList;
        for (quint64 id: sortedIds) {
            if (idsList.size() >= maxResults) {
                break;
            }
            idsList.append(QString::number(id));
        }
        handler->sendResponse(Command::SEARCH_RESULT, {idsList.join(",")});
        return;
    }


    QStringList idsList;
    for (quint64 id: baseSet) {
        if (idsList.size() >= maxResults) {
            break;
        }
        idsList.append(QString::number(id));
    }
    handler->sendResponse(Command::SEARCH_RESULT, {idsList.join(",")});
}

void ServerCore::handleGetBookSummaryRequest(ClientHandler *handler, const QStringList &fields) {
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

    if (!book->getIsActive() && !canViewInactiveBook(handler, book)) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    QString coverImageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());
    QString authorName = getAuthorName(book->getAuthorId());

    double average = averageStarsForBook(bookId);

    handler->sendResponse(Command::BOOK_SUMMARY, {
                              QString::number(book->getId()),
                              coverImageBase64,
                              book->getTitle(),
                              authorName,
                              QString::number(book->getFinalPrice()),
                              QString::number(average, 'f', 1)
                          });
}

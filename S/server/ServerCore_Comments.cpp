//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Review.h"

void ServerCore::handleGetCommentsListRequest(ClientHandler *handler, const QStringList &fields) {
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

    QSet<quint64> reviewIds = book->getReviewIds();
    QStringList idsList;
    for (quint64 reviewId: reviewIds) {
        idsList.append(formatCommentId(reviewId));
    }

    handler->sendResponse(Command::COMMENTS_LIST, {idsList.join(",")});
}

void ServerCore::handleGetCommentDetailsRequest(ClientHandler *handler, const QStringList &fields) {
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
    QString fullName = (author != nullptr) ? author->getFullName() : "";

    handler->sendResponse(Command::COMMENT_DETAILS, {
                              formatCommentId(reviewId),
                              userName,
                              fullName,
                              QString::number(review->getStars()),
                              review->getText(),
                              review->getCreatedAt().toString("yyyy/MM/dd")
                          });
}

void ServerCore::handleUpdateCommentRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 3) {
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    QString text = fields.at(1);

    bool starsOk = false;
    int stars = fields.at(2).toInt(&starsOk);
    if (!starsOk) {
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        handler->sendResponse(Command::FAIL, {});
        return;
    }


    Review *existingReview = nullptr;
    QSet<quint64> reviewIds = book->getReviewIds();
    for (quint64 reviewId: reviewIds) {
        Review *review = data.getReviewsMap().value(reviewId, nullptr);
        if (review != nullptr && review->getUserId() == userId) {
            existingReview = review;
            break;
        }
    }

    if (existingReview != nullptr) {
        existingReview->editStars(stars);
        existingReview->editText(text);
    } else {
        if (!userHasPurchasedBook(userId, bookId)) {
            handler->sendResponse(Command::FAIL, {});
            return;
        }

        Review *newReview = new Review();
        newReview->assignNewId();
        newReview->setUserId(userId);
        newReview->setBookId(bookId);
        newReview->editStars(stars);
        newReview->editText(text);

        data.getReviewsMap().insert(newReview->getId(), newReview);
        book->addReview(newReview->getId());
    }

    handler->sendResponse(Command::SUCCESS, {});


    QString bookIdText = QString::number(bookId);
    for (ClientHandler *onlineHandler: connectedClients) {
        onlineHandler->sendResponse(Command::COMMENT_UPDATED, {bookIdText});
    }
}

void ServerCore::handleDeleteCommentRequest(ClientHandler *handler, const QStringList &fields) {
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
        handler->sendResponse(Command::FAIL, {"Comment not found"});
        return;
    }


    quint64 foundReviewId = 0;
    QSet<quint64> reviewIds = book->getReviewIds();
    for (quint64 reviewId: reviewIds) {
        Review *review = data.getReviewsMap().value(reviewId, nullptr);
        if (review != nullptr && review->getUserId() == userId) {
            foundReviewId = reviewId;
            break;
        }
    }

    if (foundReviewId == 0) {
        handler->sendResponse(Command::FAIL, {"Comment not found"});
        return;
    }

    Review *review = data.getReviewsMap().value(foundReviewId, nullptr);
    data.getReviewsMap().remove(foundReviewId);
    book->removeReview(foundReviewId);
    delete review;

    handler->sendResponse(Command::SUCCESS, {"Comment deleted"});
}

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
        emit logMessageGenerated("UPDATE_COMMENT FAIL: user not authenticated");
        return;
    }

    if (fields.size() < 3) {
        emit logMessageGenerated(
            QString("UPDATE_COMMENT FAIL: invalid fields count=%1 (need bookId||text||stars), userId=%2")
                .arg(fields.size()).arg(userId));
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    QString text = fields.at(2);

    bool starsOk = false;
    int stars = fields.at(1).toInt(&starsOk);
    if (!starsOk) {
        emit logMessageGenerated(
            QString("UPDATE_COMMENT FAIL: stars not an integer (got '%1'), bookId=%2, userId=%3")
                .arg(fields.at(2)).arg(bookId).arg(userId));
        handler->sendResponse(Command::FAIL, {});
        return;
    }

    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        emit logMessageGenerated(
            QString("UPDATE_COMMENT FAIL: book not found, bookId=%1, userId=%2")
                .arg(bookId).arg(userId));
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
        emit logMessageGenerated(
            QString("UPDATE_COMMENT SUCCESS: edited existing review, bookId=%1, userId=%2, stars=%3")
                .arg(bookId).arg(userId).arg(stars));
    } else {
        if (!userHasPurchasedBook(userId, bookId)) {
            emit logMessageGenerated(
                QString("UPDATE_COMMENT FAIL: user has not purchased book (cannot add first comment), bookId=%1, userId=%2")
                    .arg(bookId).arg(userId));
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
        emit logMessageGenerated(
            QString("UPDATE_COMMENT SUCCESS: created new review id=%1, bookId=%2, userId=%3, stars=%4")
                .arg(newReview->getId()).arg(bookId).arg(userId).arg(stars));
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

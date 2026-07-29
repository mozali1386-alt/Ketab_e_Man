//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Author.h"
#include <QFile>


static const qint64 ADMIN_PDF_CHUNK_RAW_SIZE = 500 * 1024;

void ServerCore::handleGetAllBooksAdminRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    QMap<quint64, Book *> &books = data.getBooksMap();
    if (books.isEmpty()) {
        handler->sendResponse(Command::ALL_BOOKS_ADMIN_RESULT, {"0"});
        return;
    }

    QStringList idsList;
    for (auto it = books.constBegin(); it != books.constEnd(); ++it) {
        if (it.value()->getIsDeletedByAdmin()) {
            continue;
        }
        idsList.append(QString::number(it.key()));
    }
    if (idsList.isEmpty()) {
        handler->sendResponse(Command::ALL_BOOKS_ADMIN_RESULT, {"0"});
        return;
    }
    handler->sendResponse(Command::ALL_BOOKS_ADMIN_RESULT, {QString::number(idsList.size()), idsList.join(",")});
}

void ServerCore::handleGetBookInfoAdminRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getIsDeletedByAdmin()) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    QString authorName = getAuthorName(book->getAuthorId());

    User *publisherUser = data.getUsersMap().value(book->getPublisherId(), nullptr);
    QString publisherName = (publisherUser != nullptr) ? publisherUser->getFullName() : "";

    handler->sendResponse(Command::BOOK_INFO_ADMIN_RESULT, {
                              QString::number(bookId),
                              book->getTitle(),
                              authorName,
                              publisherName,
                              genreToString(book->getGenre()),
                              book->getDescription(),
                              book->getIsActive() ? "ACTIVE" : "INACTIVE"
                          });
}

void ServerCore::handleEditBookAdminRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 5) {
        handler->sendResponse(Command::EDIT_BOOK_ADMIN_RESULT, {"0", "FAILED"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getIsDeletedByAdmin()) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    QString newName = fields.at(1);
    QString newAuthor = fields.at(2);

    Genre newGenre;
    if (!stringToGenre(fields.at(3), newGenre)) {
        handler->sendResponse(Command::EDIT_BOOK_ADMIN_RESULT, {QString::number(bookId), "FAILED"});
        return;
    }

    QString newDesc = fields.at(4);

    quint64 oldAuthorId = book->getAuthorId();
    quint64 newAuthorId = findOrCreateAuthorByName(newAuthor);
    if (newAuthorId != oldAuthorId) {
        Author *oldAuthorObj = data.getAuthorsMap().value(oldAuthorId, nullptr);
        if (oldAuthorObj != nullptr) {
            oldAuthorObj->removeBook(bookId);
        }
        Author *newAuthorObj = data.getAuthorsMap().value(newAuthorId, nullptr);
        if (newAuthorObj != nullptr) {
            newAuthorObj->addBook(bookId);
        }
        book->setAuthorId(newAuthorId);
    }

    book->setTitle(newName);
    book->setGenre(newGenre);
    book->setDescription(newDesc);

    handler->sendResponse(Command::EDIT_BOOK_ADMIN_RESULT, {
                              QString::number(bookId),
                              "SUCCESS",
                              newName,
                              newAuthor,
                              genreToString(newGenre),
                              newDesc
                          });
}

void ServerCore::handleGetBookCoverRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getIsDeletedByAdmin()) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    QString imageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());

    handler->sendResponse(Command::BOOK_COVER_RESULT, {QString::number(bookId), imageBase64});
}

void ServerCore::handleDownloadPdfAdminRequest(ClientHandler *handler, const QStringList &fields) {
    User *admin = requireRole(handler, Role::ADMIN);
    if (admin == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getIsDeletedByAdmin()) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }else if (book->getPdfFilePath().isEmpty()) {
        handler->sendResponse(Command::FAIL, {"PDF not available"});
        return;
    }

    QFile file(book->getPdfFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        handler->sendResponse(Command::FAIL, {"PDF not available"});
        return;
    }

    QByteArray fileBytes = file.readAll();
    file.close();

    int totalChunks = (fileBytes.size() + ADMIN_PDF_CHUNK_RAW_SIZE - 1) / ADMIN_PDF_CHUNK_RAW_SIZE;
    if (totalChunks == 0) {
        totalChunks = 1;
    }

    handler->sendResponse(Command::PDF_START, {QString::number(bookId), QString::number(totalChunks)});

    for (qint64 offset = 0; offset < fileBytes.size(); offset += ADMIN_PDF_CHUNK_RAW_SIZE) {
        QByteArray rawChunk = fileBytes.mid(offset, ADMIN_PDF_CHUNK_RAW_SIZE);
        QString base64Chunk = QString::fromUtf8(rawChunk.toBase64());
        handler->sendResponse(Command::PDF_CHUNK, {QString::number(bookId), base64Chunk});
    }

    if (fileBytes.isEmpty()) {
        handler->sendResponse(Command::PDF_CHUNK, {QString::number(bookId), ""});
    }

    handler->sendResponse(Command::PDF_END, {QString::number(bookId), "SUCCESS"});
}

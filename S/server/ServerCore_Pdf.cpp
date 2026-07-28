//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include <QFile>


static const qint64 PDF_CHUNK_RAW_SIZE = 500 * 1024;

void ServerCore::handleDownloadBookRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();

    if (!userHasPurchasedBook(userId, bookId)) {
        handler->sendResponse(Command::FAIL, {"You have not purchased this book"});
        return;
    }

    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPdfFilePath().isEmpty()) {
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

    int totalChunks = (fileBytes.size() + PDF_CHUNK_RAW_SIZE - 1) / PDF_CHUNK_RAW_SIZE;
    if (totalChunks == 0) {
        totalChunks = 1;
    }

    Library *library = getLibraryForUser(userId);
    int lastReadPage = (library != nullptr) ? library->getLastReadPage(bookId) : 0;

    handler->sendResponse(Command::FILE_START, {
                              QString::number(bookId),
                              QString::number(totalChunks),
                              QString::number(lastReadPage)
                          });

    for (qint64 offset = 0; offset < fileBytes.size(); offset += PDF_CHUNK_RAW_SIZE) {
        QByteArray rawChunk = fileBytes.mid(offset, PDF_CHUNK_RAW_SIZE);
        QString base64Chunk = QString::fromUtf8(rawChunk.toBase64());
        handler->sendResponse(Command::FILE_CHUNK, {QString::number(bookId), base64Chunk});
    }

    if (fileBytes.isEmpty()) {
        handler->sendResponse(Command::FILE_CHUNK, {QString::number(bookId), ""});
    }

    handler->sendResponse(Command::FILE_END, {QString::number(bookId)});
}

void ServerCore::handleUpdateLastPageRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 2) {
        handler->sendResponse(Command::UPDATE_LAST_PAGE, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    int pageNumber = fields.at(1).toInt();

    if (!userHasPurchasedBook(userId, bookId)) {
        handler->sendResponse(Command::UPDATE_LAST_PAGE, {"FAIL"});
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        handler->sendResponse(Command::UPDATE_LAST_PAGE, {"FAIL"});
        return;
    }

    library->setLastReadPage(bookId, pageNumber);

    handler->sendResponse(Command::UPDATE_LAST_PAGE, {"SUCCESS"});
}

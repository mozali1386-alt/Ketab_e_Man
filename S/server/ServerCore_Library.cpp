//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Shelf.h"

void ServerCore::handleGetMyBooksRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        handler->sendResponse(Command::FAIL, {"Library not found"});
        return;
    }

    QSet<quint64> bookIds = library->getPurchasedBooks();
    QStringList idsList;
    for (quint64 bookId: bookIds) {
        idsList.append(QString::number(bookId));
    }

    handler->sendResponse(Command::MY_BOOKS_RESULT, {QString::number(bookIds.size()), idsList.join(",")});
}

void ServerCore::handleGetSavedBooksRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        handler->sendResponse(Command::FAIL, {"Library not found"});
        return;
    }

    QSet<quint64> bookIds = library->getSavedBooks();
    QStringList idsList;
    for (quint64 bookId: bookIds) {
        idsList.append(QString::number(bookId));
    }

    handler->sendResponse(Command::SAVED_BOOKS_RESULT, {QString::number(bookIds.size()), idsList.join(",")});
}

void ServerCore::handleGetShelvesRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        handler->sendResponse(Command::FAIL, {"Library not found"});
        return;
    }

    QSet<quint64> shelfIds = library->getShelves();
    QStringList entries;
    for (quint64 shelfId: shelfIds) {
        Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
        if (shelf == nullptr) {
            continue;
        }
        entries.append(QString::number(shelfId) + ":" + shelf->getName());
    }

    handler->sendResponse(Command::SHELVES_RESULT, {QString::number(entries.size()), entries.join(",")});
}

void ServerCore::handleGetShelfBooksRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 shelfId = fields.at(0).toULongLong();
    Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
    if (shelf == nullptr || shelf->getOwnerId() != userId) {
        handler->sendResponse(Command::FAIL, {"Shelf not found"});
        return;
    }

    QSet<quint64> bookIds = shelf->getBookIds();
    if (bookIds.isEmpty()) {
        handler->sendResponse(Command::SHELF_BOOKS_RESULT, {QString::number(shelfId), "EMPTY"});
        return;
    }

    QStringList idsList;
    for (quint64 bookId: bookIds) {
        idsList.append(QString::number(bookId));
    }

    handler->sendResponse(Command::SHELF_BOOKS_RESULT, {QString::number(shelfId), idsList.join(",")});
}

void ServerCore::handleGetMyBookInfoRequest(ClientHandler *handler, const QStringList &fields) {
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

    QString imageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());

    quint64 currentShelfId = findShelfContainingBook(userId, bookId);

    handler->sendResponse(Command::MY_BOOK_INFO_RESULT, {
                              QString::number(bookId),
                              imageBase64,
                              book->getTitle(),
                              QString::number(currentShelfId)
                          });
}

void ServerCore::handleGetSavedBookInfoRequest(ClientHandler *handler, const QStringList &fields) {
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

    QString imageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());

    bool purchased = userHasPurchasedBook(userId, bookId);

    handler->sendResponse(Command::SAVED_BOOK_INFO_RESULT, {
                              QString::number(bookId),
                              imageBase64,
                              book->getTitle(),
                              purchased ? "TRUE" : "FALSE"
                          });
}

void ServerCore::handleGetShelfBookInfoRequest(ClientHandler *handler, const QStringList &fields) {
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

    QString imageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());

    quint64 currentShelfId = findShelfContainingBook(userId, bookId);

    handler->sendResponse(Command::SHELF_BOOK_INFO_RESULT, {
                              QString::number(bookId),
                              imageBase64,
                              book->getTitle(),
                              QString::number(currentShelfId)
                          });
}

void ServerCore::handleAddShelfRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::ADD_SHELF_RESULT, {"FAIL"});
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        handler->sendResponse(Command::ADD_SHELF_RESULT, {"FAIL"});
        return;
    }

    Shelf *shelf = new Shelf();
    shelf->assignNewId();
    shelf->setOwnerId(userId);
    shelf->rename(fields.at(0));
    data.getShelvesMap().insert(shelf->getId(), shelf);

    library->addShelf(shelf->getId());

    handler->sendResponse(Command::ADD_SHELF_RESULT, {"SUCCESS"});
}

void ServerCore::handleEditShelfRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 2) {
        handler->sendResponse(Command::EDIT_SHELF_RESULT, {"FAIL"});
        return;
    }

    quint64 shelfId = fields.at(0).toULongLong();
    Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
    if (shelf == nullptr || shelf->getOwnerId() != userId) {
        handler->sendResponse(Command::EDIT_SHELF_RESULT, {"FAIL"});
        return;
    }

    shelf->rename(fields.at(1));

    handler->sendResponse(Command::EDIT_SHELF_RESULT, {"SUCCESS"});
}

void ServerCore::handleDeleteShelfRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::DELETE_SHELF_RESULT, {"FAIL"});
        return;
    }

    quint64 shelfId = fields.at(0).toULongLong();
    Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
    if (shelf == nullptr || shelf->getOwnerId() != userId) {
        handler->sendResponse(Command::DELETE_SHELF_RESULT, {"FAIL"});
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library != nullptr) {
        library->removeShelf(shelfId);
    }

    data.getShelvesMap().remove(shelfId);
    delete shelf;

    handler->sendResponse(Command::DELETE_SHELF_RESULT, {"SUCCESS"});
}

void ServerCore::handleUnsaveBookRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::REMOVE_SAVED_RESULT, {"FAIL"});
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        handler->sendResponse(Command::REMOVE_SAVED_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    library->removeFromSavedBooks(bookId);

    handler->sendResponse(Command::REMOVE_SAVED_RESULT, {"SUCCESS"});
}

void ServerCore::handleRemoveFromShelfRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 2) {
        handler->sendResponse(Command::REMOVE_FROM_SHELF_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    quint64 shelfId = fields.at(1).toULongLong();

    Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
    if (shelf == nullptr || shelf->getOwnerId() != userId) {
        handler->sendResponse(Command::REMOVE_FROM_SHELF_RESULT, {"FAIL"});
        return;
    }

    shelf->removeBook(bookId);

    handler->sendResponse(Command::REMOVE_FROM_SHELF_RESULT, {"SUCCESS"});
}

void ServerCore::handleAssignToShelfRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 2) {
        handler->sendResponse(Command::ASSIGN_SHELF_RESULT, {"FAIL"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    quint64 newShelfId = fields.at(1).toULongLong();

    Shelf *shelf = data.getShelvesMap().value(newShelfId, nullptr);
    if (shelf == nullptr || shelf->getOwnerId() != userId) {
        handler->sendResponse(Command::ASSIGN_SHELF_RESULT, {"FAIL"});
        return;
    }

    if (!userHasPurchasedBook(userId, bookId)) {
        handler->sendResponse(Command::ASSIGN_SHELF_RESULT, {"FAIL"});
        return;
    }

    quint64 currentShelfId = findShelfContainingBook(userId, bookId);
    if (currentShelfId != 0 && currentShelfId != newShelfId) {
        Shelf *currentShelf = data.getShelvesMap().value(currentShelfId, nullptr);
        if (currentShelf != nullptr) {
            currentShelf->removeBook(bookId);
        }
    }

    shelf->addBook(bookId);

    handler->sendResponse(Command::ASSIGN_SHELF_RESULT, {"SUCCESS"});
}

void ServerCore::handleSaveBookRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        handler->sendResponse(Command::FAIL, {"Library not found"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    library->addToSavedBooks(bookId);

    handler->sendResponse(Command::SUCCESS, {"Book saved"});
}

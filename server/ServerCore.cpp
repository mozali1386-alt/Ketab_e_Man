//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Protocol.h"
#include <QStringList>
#include <QtAlgorithms>

ServerCore::ServerCore(QObject *parent) : QObject(parent) {
    network = new ServerNetwork(this);

    connect(network, &ServerNetwork::newClientHandlerCreated, this, &ServerCore::onNewClientHandlerCreated);

    fileManager.loadAllData(&data);
    searchEngine.buildIndexes(data.getBooksMap());
}

ServerCore::~ServerCore() {
    stopServer();

    fileManager.saveAllData(&data);

    if (network != nullptr) {
        delete network;
        network = nullptr;
    }
}

bool ServerCore::startServer(quint16 port) {
    bool success = network->startListening(port);
    if (success) {
        emit logMessageGenerated("Server started listening on port " + QString::number(port));
    } else {
        emit logMessageGenerated("Server failed to start on port " + QString::number(port));
    }
    return success;
}

void ServerCore::stopServer() {
    dataMutex.lock();

    for (ClientHandler *handler: connectedClients) {
        handler->deleteLater();
    }
    connectedClients.clear();
    loggedInClients.clear();

    dataMutex.unlock();

    network->stopListening();

    emit connectedClientCountChanged(0);
}

ServerDataStore &ServerCore::getDataStore() {
    return data;
}

int ServerCore::getConnectedClientCount() const {
    dataMutex.lock();
    int count = connectedClients.size();
    dataMutex.unlock();
    return count;
}

int ServerCore::getTotalRequestCount() const {
    return stats.getTotalRequests();
}

void ServerCore::onNewClientHandlerCreated(ClientHandler *handler) {
    connect(handler, &ClientHandler::requestReceived, this, &ServerCore::onClientRequestReceived);
    connect(handler, &ClientHandler::clientDisconnected, this, &ServerCore::onClientDisconnected);

    dataMutex.lock();
    connectedClients.insert(handler);
    int count = connectedClients.size();
    dataMutex.unlock();

    emit logMessageGenerated("New client connected");
    emit connectedClientCountChanged(count);
}

void ServerCore::onClientRequestReceived(ClientHandler *handler, int commandId, QString payload) {
    stats.incrementTotalRequests();
    handleRequest(handler, commandId, payload);
}

void ServerCore::onClientDisconnected(ClientHandler *handler) {
    dataMutex.lock();

    connectedClients.remove(handler);
    loggedInClients.remove(handler->getUserId());

    int count = connectedClients.size();
    dataMutex.unlock();

    emit logMessageGenerated("Client disconnected");
    emit connectedClientCountChanged(count);

    handler->deleteLater();
}

void ServerCore::handleRequest(ClientHandler *handler, int commandId, const QString &payload) {
    switch (commandId) {
        case REQ_LOGIN: handleLoginRequest(handler, payload);
            break;
        case REQ_REGISTER: handleRegisterRequest(handler, payload);
            break;
        case REQ_FORGOT_PASSWORD: handleForgotPasswordRequest(handler, payload);
            break;
        case REQ_RESET_PASSWORD: handleResetPasswordRequest(handler, payload);
            break;
        case REQ_LOGOUT: handleLogoutRequest(handler, payload);
            break;
        case REQ_SET_FAVORITE_GENRES: handleSetFavoriteGenresRequest(handler, payload);
            break;
        case REQ_BLOCK_USER: handleBlockUserRequest(handler, payload);
            break;
        case REQ_UNBLOCK_USER: handleUnblockUserRequest(handler, payload);
            break;

        case REQ_PUBLISH: handlePublishRequest(handler, payload);
            break;
        case REQ_UPDATE_BOOK: handleUpdateBookRequest(handler, payload);
            break;
        case REQ_DEACTIVATE_BOOK: handleDeactivateBookRequest(handler, payload);
            break;
        case REQ_SEARCH: handleSearchRequest(handler, payload);
            break;
        case REQ_DELETE_BOOK: handleDeleteBookRequest(handler, payload);
            break;

        case REQ_BUY: handleBuyRequest(handler, payload);
            break;
        case REQ_ADD_TO_CART: handleAddToCartRequest(handler, payload);
            break;
        case REQ_REMOVE_FROM_CART: handleRemoveFromCartRequest(handler, payload);
            break;
        case REQ_CHECKOUT_CART: handleCheckoutCartRequest(handler, payload);
            break;
        case REQ_ADD_REVIEW: handleAddReviewRequest(handler, payload);
            break;
        case REQ_EDIT_REVIEW: handleEditReviewRequest(handler, payload);
            break;
        case REQ_DELETE_REVIEW: handleDeleteReviewRequest(handler, payload);
            break;

        case REQ_CREATE_SHELF: handleCreateShelfRequest(handler, payload);
            break;
        case REQ_ADD_BOOK_TO_SHELF: handleAddBookToShelfRequest(handler, payload);
            break;
        case REQ_SAVE_BOOK: handleSaveBookRequest(handler, payload);
            break;

        default: handler->sendResponse(RES_FAIL, "Unknown command");
            break;
    }
}

User *ServerCore::findUserByUsername(const QString &username) {
    QList<quint64> keys = data.getUsersMap().keys();
    for (int i = 0; i < keys.size(); i++) {
        User *user = data.getUsersMap().value(keys.at(i));
        if (user->getUsername() == username) {
            return user;
        }
    }
    return nullptr;
}

Cart *ServerCore::findOrCreateCartForUser(quint64 userId) {
    QList<quint64> keys = data.getCartsMap().keys();
    for (int i = 0; i < keys.size(); i++) {
        Cart *cart = data.getCartsMap().value(keys.at(i));
        if (cart->getOwnerId() == userId) {
            return cart;
        }
    }

    Cart *newCart = new Cart();
    newCart->assignNewId();
    newCart->setOwnerId(userId);
    data.getCartsMap().insert(newCart->getId(), newCart);
    return newCart;
}

ClientHandler *ServerCore::findClientHandlerByUserId(quint64 userId) {
    dataMutex.lock();
    ClientHandler *handler = loggedInClients.value(userId, nullptr);
    dataMutex.unlock();
    return handler;
}

bool ServerCore::splitPayloadOrFail(ClientHandler *handler, const QString &payload, int minParts,
                                    QStringList &outParts) {
    outParts = payload.split("|");
    if (outParts.size() < minParts) {
        handler->sendResponse(RES_FAIL, "Invalid data");
        return false;
    }
    return true;
}

bool ServerCore::parseGenre(const QString &text, Genre &out) {
    bool ok = false;
    int value = text.toInt(&ok);
    if (!ok || value < static_cast<int>(Genre::FICTION) || value > static_cast<int>(Genre::TECH)) {
        return false;
    }
    out = static_cast<Genre>(value);
    return true;
}

bool ServerCore::parseDouble(const QString &text, double &out) {
    bool ok = false;
    out = text.toDouble(&ok);
    return ok;
}

quint64 ServerCore::requireAuthentication(ClientHandler *handler) {
    if (!handler->isAuthenticated()) {
        handler->sendResponse(RES_FAIL, "Not authenticated");
        return 0;
    }
    return handler->getUserId();
}

Admin *ServerCore::requireAdmin(quint64 userId, ClientHandler *handler) {
    User *user = data.getUsersMap().value(userId, nullptr);
    if (user == nullptr || user->getRole() != Role::ADMIN) {
        handler->sendResponse(RES_FAIL, "Not authorized");
        return nullptr;
    }
    return static_cast<Admin *>(user);
}

NormalUser *ServerCore::requireNormalUser(quint64 userId, ClientHandler *handler) {
    User *user = data.getUsersMap().value(userId, nullptr);
    if (user == nullptr || user->getRole() != Role::USER) {
        handler->sendResponse(RES_FAIL, "Not authorized");
        return nullptr;
    }
    return static_cast<NormalUser *>(user);
}

Library *ServerCore::requireUserLibrary(quint64 userId, ClientHandler *handler) {
    NormalUser *normalUser = requireNormalUser(userId, handler);
    if (normalUser == nullptr) {
        return nullptr;
    }

    Library *library = data.getLibrariesMap().value(normalUser->getLibraryId(), nullptr);
    if (library == nullptr) {
        handler->sendResponse(RES_FAIL, "Library not found");
        return nullptr;
    }
    return library;
}

Book *ServerCore::requireOwnedBook(quint64 bookId, quint64 userId, ClientHandler *handler) {
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPublisherId() != userId) {
        handler->sendResponse(RES_FAIL, "Not authorized");
        return nullptr;
    }

    User *publisherUser = data.getUsersMap().value(userId, nullptr);
    if (publisherUser == nullptr || publisherUser->getRole() != Role::PUBLISHER) {
        handler->sendResponse(RES_FAIL, "Not authorized");
        return nullptr;
    }

    return book;
}

void ServerCore::sendNotificationToUser(quint64 userId, NotificationType type, const QString &message) {
    Notification *notification = new Notification();
    notification->assignNewId();
    notification->setRecipientId(userId);
    notification->setMessage(message);
    notification->setType(type);
    data.getNotificationsMap().insert(notification->getId(), notification);

    User *user = data.getUsersMap().value(userId, nullptr);
    if (user != nullptr) {
        user->addNotification(notification->getId());
    }

    ClientHandler *targetHandler = findClientHandlerByUserId(userId);
    if (targetHandler != nullptr) {
        QString payload = QString::number(static_cast<int>(type)) + "|" + message;
        targetHandler->sendResponse(NOTIFY_NEW_EVENT, payload);
    }
}
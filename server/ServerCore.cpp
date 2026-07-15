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

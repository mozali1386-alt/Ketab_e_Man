//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Protocol.h"
#include "../shared/Publisher.h"
#include <QStringList>

void ServerCore::handleLoginRequest(ClientHandler *handler, const QString &payload) {
    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 2, parts)) {
        return;
    }

    User *user = findUserByUsername(parts.at(0));
    if (user == nullptr || user->getIsBlocked() || !user->authenticatePassword(parts.at(1))) {
        handler->sendResponse(RES_FAIL, "Invalid username or password");
        return;
    }

    ClientHandler *existingSession = loggedInClients.value(user->getId(), nullptr);
    if (existingSession != nullptr && existingSession != handler) {
        existingSession->setUserId(0);
        existingSession->sendResponse(RES_FAIL, "Logged in from another location");
    }

    handler->setUserId(user->getId());
    loggedInClients.insert(user->getId(), handler);

    QString response = QString::number(user->getId()) + "|" + QString::number(static_cast<int>(user->getRole()));
    handler->sendResponse(RES_LOGIN_OK, response);
}

void ServerCore::handleRegisterRequest(ClientHandler *handler, const QString &payload) {
    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 7, parts)) {
        return;
    }

    bool roleOk = false;
    int roleInt = parts.at(0).toInt(&roleOk);
    QString fullName = parts.at(1);
    QString username = parts.at(2);

    if (!roleOk) {
        handler->sendResponse(RES_FAIL, "Invalid role");
        return;
    }

    if (findUserByUsername(username) != nullptr) {
        handler->sendResponse(RES_FAIL, "Username already exists");
        return;
    }

    User *newUser = nullptr;
    if (roleInt == static_cast<int>(Role::PUBLISHER)) {
        newUser = new Publisher();
    } else if (roleInt == static_cast<int>(Role::USER)) {
        newUser = new NormalUser();
    } else {
        handler->sendResponse(RES_FAIL, "Invalid role");
        return;
    }

    newUser->assignNewId();
    newUser->setFullName(fullName);
    newUser->setUsername(username);
    newUser->setEmail(parts.at(3));
    newUser->setPassword(parts.at(4));
    newUser->setSecurityQuestion(parts.at(5));
    newUser->setSecurityAnswer(parts.at(6));
    newUser->setIsBlocked(false);
    data.getUsersMap().insert(newUser->getId(), newUser);

    Wallet *wallet = new Wallet();
    wallet->assignNewId();
    wallet->setOwnerId(newUser->getId());
    data.getWalletsMap().insert(wallet->getId(), wallet);
    newUser->setWalletId(wallet->getId());

    if (roleInt != static_cast<int>(Role::PUBLISHER)) {
        Library *library = new Library();
        library->assignNewId();
        library->setOwnerId(newUser->getId());
        data.getLibrariesMap().insert(library->getId(), library);
        static_cast<NormalUser *>(newUser)->setLibraryId(library->getId());
    }

    handler->sendResponse(RES_REGISTER_OK, QString::number(newUser->getId()));
}

void ServerCore::handleForgotPasswordRequest(ClientHandler *handler, const QString &payload) {
    User *user = findUserByUsername(payload);
    if (user == nullptr) {
        handler->sendResponse(RES_FAIL, "User not found");
        return;
    }
    handler->sendResponse(RES_SUCCESS, user->getSecurityQuestion());
}

void ServerCore::handleResetPasswordRequest(ClientHandler *handler, const QString &payload) {
    QStringList parts;
    if (!splitPayloadOrFail(handler, payload, 4, parts)) {
        return;
    }

    User *user = findUserByUsername(parts.at(0));
    if (user == nullptr) {
        handler->sendResponse(RES_FAIL, "User not found");
        return;
    }

    if (user->resetPassword(parts.at(1), parts.at(2), parts.at(3))) {
        handler->sendResponse(RES_SUCCESS, "Password reset");
    } else {
        handler->sendResponse(RES_FAIL, "Incorrect security answer");
    }
}

void ServerCore::handleLogoutRequest(ClientHandler *handler, const QString &payload) {
    Q_UNUSED(payload);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    loggedInClients.remove(userId);
    handler->setUserId(0);
    handler->sendResponse(RES_SUCCESS, "Logged out");
}

void ServerCore::handleSetFavoriteGenresRequest(ClientHandler *handler, const QString &payload) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    NormalUser *normalUser = requireNormalUser(userId, handler);
    if (normalUser == nullptr) {
        return;
    }

    QStringList genreParts = payload.split(",");
    for (int i = 0; i < genreParts.size(); i++) {
        Genre genre;
        if (parseGenre(genreParts.at(i), genre)) {
            normalUser->addFavoriteGenre(genre);
        }
    }

    handler->sendResponse(RES_SUCCESS, "Favorite genres updated");
}

void ServerCore::handleBlockUserRequest(ClientHandler *handler, const QString &payload) {
    quint64 adminId = requireAuthentication(handler);
    if (adminId == 0) {
        return;
    }

    Admin *admin = requireAdmin(adminId, handler);
    if (admin == nullptr) {
        return;
    }

    User *target = data.getUsersMap().value(payload.toULongLong(), nullptr);
    if (target == nullptr) {
        handler->sendResponse(RES_FAIL, "User not found");
        return;
    }

    admin->blockUser(target);
    handler->sendResponse(RES_SUCCESS, "User blocked");
}

void ServerCore::handleUnblockUserRequest(ClientHandler *handler, const QString &payload) {
    quint64 adminId = requireAuthentication(handler);
    if (adminId == 0) {
        return;
    }

    Admin *admin = requireAdmin(adminId, handler);
    if (admin == nullptr) {
        return;
    }

    User *target = data.getUsersMap().value(payload.toULongLong(), nullptr);
    if (target == nullptr) {
        handler->sendResponse(RES_FAIL, "User not found");
        return;
    }

    admin->unblockUser(target);
    handler->sendResponse(RES_SUCCESS, "User unblocked");
}

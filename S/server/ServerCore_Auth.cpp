//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Publisher.h"
#include "../shared/Wallet.h"
#include "../shared/Library.h"

void ServerCore::handleLoginRequest(ClientHandler *handler, const QStringList &fields) {
    if (fields.size() < 3) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    bool roleOk = false;
    Role requestedRole = stringToRole(fields.at(0), roleOk);
    QString username = fields.at(1);
    QString password = fields.at(2);

    if (!roleOk) {
        handler->sendResponse(Command::LOGIN, {"FAIL", "Invalid username or password"});
        return;
    }

    User *user = findUserByUsername(username);
    if (user == nullptr || user->getIsBlocked() || user->getRole() != requestedRole || !user->
        authenticatePassword(password)) {
        handler->sendResponse(Command::LOGIN, {"FAIL", "Invalid username or password"});
        return;
    }

    ClientHandler *existingSession = loggedInClients.value(user->getId(), nullptr);
    if (existingSession != nullptr && existingSession != handler) {
        existingSession->setUserId(0);
        existingSession->sendResponse(Command::LOGIN, {"FAIL", "Logged in from another location"});
    }

    handler->setUserId(user->getId());
    loggedInClients.insert(user->getId(), handler);

    handler->sendResponse(Command::LOGIN, {"SUCCESS", QString::number(user->getId())});

    pushAdminStatsUpdate();
}

void ServerCore::handleSignupPublisherRequest(ClientHandler *handler, const QStringList &fields) {
    if (fields.size() < 4) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    QString publisherName = fields.at(0);
    QString username = fields.at(1);
    QString email = fields.at(2);
    QString password = fields.at(3);

    if (findUserByUsername(username) != nullptr) {
        handler->sendResponse(Command::SIGNUP_PUBLISHER, {"FAIL", "USERNAME_EXISTS"});
        return;
    }

    if (data.findUserByEmail(email) != nullptr) {
        handler->sendResponse(Command::SIGNUP_PUBLISHER, {"FAIL", "EMAIL_EXISTS"});
        return;
    }

    Publisher *newPublisher = new Publisher();
    newPublisher->assignNewId();
    newPublisher->setFullName(publisherName);
    newPublisher->setUsername(username);
    newPublisher->setEmail(email);
    newPublisher->setPassword(password);
    newPublisher->setIsBlocked(false);

    data.getUsersMap().insert(newPublisher->getId(), newPublisher);
    data.registerUsername(username, newPublisher->getId());
    data.registerEmail(email, newPublisher->getId());

    Wallet *wallet = new Wallet();
    wallet->assignNewId();
    wallet->setOwnerId(newPublisher->getId());
    data.getWalletsMap().insert(wallet->getId(), wallet);
    newPublisher->setWalletId(wallet->getId());

    handler->sendResponse(Command::SIGNUP_PUBLISHER, {"SUCCESS"});

    pushAdminStatsUpdate();
}

void ServerCore::handleSignupNormalUserRequest(ClientHandler *handler, const QStringList &fields) {
    if (fields.size() < 5) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    QString fullName = fields.at(0);
    QString username = fields.at(1);
    QString email = fields.at(2);
    QString password = fields.at(3);
    QString genresString = fields.at(4);

    if (findUserByUsername(username) != nullptr) {
        handler->sendResponse(Command::SIGNUP_NORMALUSER, {"FAIL", "USERNAME_EXISTS"});
        return;
    }

    if (data.findUserByEmail(email) != nullptr) {
        handler->sendResponse(Command::SIGNUP_NORMALUSER, {"FAIL", "EMAIL_EXISTS"});
        return;
    }

    NormalUser *newUser = new NormalUser();
    newUser->assignNewId();
    newUser->setFullName(fullName);
    newUser->setUsername(username);
    newUser->setEmail(email);
    newUser->setPassword(password);
    newUser->setIsBlocked(false);

    QStringList genreParts = genresString.split(",");
    for (int i = 0; i < genreParts.size(); i++) {
        Genre genre;
        if (stringToGenre(genreParts.at(i), genre)) {
            newUser->addFavoriteGenre(genre);
        }
    }

    data.getUsersMap().insert(newUser->getId(), newUser);
    data.registerUsername(username, newUser->getId());
    data.registerEmail(email, newUser->getId());

    Wallet *wallet = new Wallet();
    wallet->assignNewId();
    wallet->setOwnerId(newUser->getId());
    data.getWalletsMap().insert(wallet->getId(), wallet);
    newUser->setWalletId(wallet->getId());

    Library *library = new Library();
    library->assignNewId();
    library->setOwnerId(newUser->getId());
    data.getLibrariesMap().insert(library->getId(), library);
    newUser->setLibraryId(library->getId());

    handler->sendResponse(Command::SIGNUP_NORMALUSER, {"SUCCESS"});

    pushAdminStatsUpdate();
}

void ServerCore::handleLogoutRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    loggedInClients.remove(userId);
    handler->setUserId(0);
    handler->sendResponse(Command::SUCCESS, {"Logged out"});

    pushAdminStatsUpdate();
}

void ServerCore::handleForgotPassCheckRequest(ClientHandler *handler, const QStringList &fields) {
    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    QString email = fields.at(0);
    User *user = data.findUserByEmail(email);
    if (user == nullptr) {
        handler->sendResponse(Command::FORGOT_PASS_CHECK, {"FAIL"});
        return;
    }

    handler->setRecoveryUserId(user->getId());
    handler->sendResponse(Command::FORGOT_PASS_CHECK, {"SUCCESS", user->getUsername()});
}

void ServerCore::handleForgotPassUpdateRequest(ClientHandler *handler, const QStringList &fields) {
    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 targetUserId = handler->getRecoveryUserId();
    if (targetUserId == 0) {
        targetUserId = handler->getUserId();
    }
    if (targetUserId == 0) {
        handler->sendResponse(Command::FORGOT_PASS_UPDATE, {"FAIL"});
        return;
    }

    User *user = data.getUsersMap().value(targetUserId, nullptr);
    if (user == nullptr) {
        handler->setRecoveryUserId(0);
        handler->sendResponse(Command::FORGOT_PASS_UPDATE, {"FAIL"});
        return;
    }

    QString newPassword = fields.at(0);
    user->setPassword(newPassword);
    handler->setRecoveryUserId(0);

    handler->sendResponse(Command::FORGOT_PASS_UPDATE, {"SUCCESS"});
}

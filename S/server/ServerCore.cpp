//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Author.h"
#include "../shared/Admin.h"
#include "../shared/Cart.h"
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QCoreApplication>

ServerCore::ServerCore(QObject *parent) : QObject(parent) {
    network = new ServerNetwork(this);

    connect(network, &ServerNetwork::newClientHandlerCreated, this, &ServerCore::onNewClientHandlerCreated);

    fileManager.loadAllData(&data);
    data.rebuildAllIndexes();

    ensureDefaultAdminExists();
}

void ServerCore::ensureDefaultAdminExists() {
    QMap<quint64, User *> &users = data.getUsersMap();

    bool adminExists = false;
    for (auto it = users.constBegin(); it != users.constEnd(); ++it) {
        if (it.value()->getRole() == Role::ADMIN) {
            adminExists = true;
            break;
        }
    }

    if (adminExists) {
        return;
    }

    Admin *admin = new Admin();
    admin->assignNewId();
    admin->setFullName("Administrator");
    admin->setUsername("admin");
    admin->setEmail("admin@ketabeman.local");
    admin->setPassword("admin123");
    admin->setIsBlocked(false);

    data.getUsersMap().insert(admin->getId(), admin);
    data.registerUsername("admin", admin->getId());
    data.registerEmail("admin@ketabeman.local", admin->getId());

    Wallet *wallet = new Wallet();
    wallet->assignNewId();
    wallet->setOwnerId(admin->getId());
    data.getWalletsMap().insert(wallet->getId(), wallet);
    admin->setWalletId(wallet->getId());

    emit logMessageGenerated("Default admin account created (username: admin, password: admin123)");
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

void ServerCore::onClientRequestReceived(ClientHandler *handler, Command command, QStringList fields) {
    stats.incrementTotalRequests();
    handleRequest(handler, command, fields);
}

void ServerCore::onClientDisconnected(ClientHandler *handler) {
    dataMutex.lock();

    connectedClients.remove(handler);
    loggedInClients.remove(handler->getUserId());


    quint64 disconnectedUserId = handler->getUserId();
    if (disconnectedUserId != 0) {
        QList<quint64> pendingBookIds = pdfUploads.keys();
        for (int i = 0; i < pendingBookIds.size(); i++) {
            quint64 bookId = pendingBookIds.at(i);
            if (pdfUploads.value(bookId).uploaderId == disconnectedUserId) {
                pdfUploads.remove(bookId);
            }
        }
    }

    int count = connectedClients.size();
    dataMutex.unlock();

    emit logMessageGenerated("Client disconnected");
    emit connectedClientCountChanged(count);

    pushAdminStatsUpdate();

    handler->deleteLater();
}

void ServerCore::handleRequest(ClientHandler *handler, Command command, const QStringList &fields) {
    switch (command) {
        case Command::LOGIN: handleLoginRequest(handler, fields);
            break;
        case Command::SIGNUP_PUBLISHER: handleSignupPublisherRequest(handler, fields);
            break;
        case Command::SIGNUP_NORMALUSER: handleSignupNormalUserRequest(handler, fields);
            break;
        case Command::LOGOUT: handleLogoutRequest(handler, fields);
            break;
        case Command::FORGOT_PASS_CHECK: handleForgotPassCheckRequest(handler, fields);
            break;
        case Command::FORGOT_PASS_UPDATE: handleForgotPassUpdateRequest(handler, fields);
            break;

        case Command::GET_PUBLISHER_BOOKS: handleGetPublisherBooksRequest(handler, fields);
            break;
        case Command::GET_PUB_BOOK_DETAILS: handleGetPubBookDetailsRequest(handler, fields);
            break;
        case Command::GET_BOOK_DETAILS: handleGetBookDetailsRequest(handler, fields);
            break;
        case Command::ADD_BOOK_METADATA: handleAddBookMetadataRequest(handler, fields);
            break;
        case Command::UPLOAD_PDF_START: handleUploadPdfStartRequest(handler, fields);
            break;
        case Command::UPLOAD_PDF_CHUNK: handleUploadPdfChunkRequest(handler, fields);
            break;
        case Command::UPLOAD_PDF_END: handleUploadPdfEndRequest(handler, fields);
            break;
        case Command::EDIT_BOOK: handleEditBookRequest(handler, fields);
            break;
        case Command::TOGGLE_BOOK_STATUS: handleToggleBookStatusRequest(handler, fields);
            break;
        case Command::DELETE_BOOK: handleDeleteBookRequest(handler, fields);
            break;
        case Command::SEARCH_STOREPAGE: handleSearchStorepageRequest(handler, fields);
            break;
        case Command::GET_BOOK_SUMMARY: handleGetBookSummaryRequest(handler, fields);
            break;

        case Command::BUY: handleBuyRequest(handler, fields);
            break;
        case Command::GET_CART_ITEMS: handleGetCartItemsRequest(handler, fields);
            break;
        case Command::GET_CART_BOOK_SUMMARY: handleGetCartBookSummaryRequest(handler, fields);
            break;
        case Command::ADD_CART: handleAddCartRequest(handler, fields);
            break;
        case Command::REMOVE_CART: handleRemoveCartRequest(handler, fields);
            break;
        case Command::CHECKOUT_CART: handleCheckoutCartRequest(handler, fields);
            break;

        case Command::GET_MY_BOOKS: handleGetMyBooksRequest(handler, fields);
            break;
        case Command::GET_SAVED_BOOKS: handleGetSavedBooksRequest(handler, fields);
            break;
        case Command::GET_SHELVES: handleGetShelvesRequest(handler, fields);
            break;
        case Command::GET_SHELF_BOOKS: handleGetShelfBooksRequest(handler, fields);
            break;
        case Command::GET_MY_BOOK_INFO: handleGetMyBookInfoRequest(handler, fields);
            break;
        case Command::GET_SAVED_BOOK_INFO: handleGetSavedBookInfoRequest(handler, fields);
            break;
        case Command::GET_SHELF_BOOK_INFO: handleGetShelfBookInfoRequest(handler, fields);
            break;
        case Command::ADD_SHELF: handleAddShelfRequest(handler, fields);
            break;
        case Command::EDIT_SHELF: handleEditShelfRequest(handler, fields);
            break;
        case Command::DELETE_SHELF: handleDeleteShelfRequest(handler, fields);
            break;
        case Command::UNSAVE_BOOK: handleUnsaveBookRequest(handler, fields);
            break;
        case Command::REMOVE_FROM_SHELF: handleRemoveFromShelfRequest(handler, fields);
            break;
        case Command::ASSIGN_TO_SHELF: handleAssignToShelfRequest(handler, fields);
            break;
        case Command::SAVE_BOOK: handleSaveBookRequest(handler, fields);
            break;

        case Command::GET_PUB_GENERAL_STATS: handleGetPubGeneralStatsRequest(handler, fields);
            break;
        case Command::GET_PUB_TOP_BOOKS_IDS: handleGetPubTopBooksIdsRequest(handler, fields);
            break;
        case Command::GET_PUB_BOOK_INFO_TOP: handleGetPubBookInfoTopRequest(handler, fields);
            break;
        case Command::GET_PUB_LOWEST_BOOKS_IDS: handleGetPubLowestBooksIdsRequest(handler, fields);
            break;
        case Command::GET_PUB_BOOK_INFO_LOWEST: handleGetPubBookInfoLowestRequest(handler, fields);
            break;
        case Command::GET_PUB_SALES_CHART: handleGetPubSalesChartRequest(handler, fields);
            break;
        case Command::GET_PUB_SCORE_CHART: handleGetPubScoreChartRequest(handler, fields);
            break;

        case Command::GET_PUB_PROFILE_INFO: handleGetPubProfileInfoRequest(handler, fields);
            break;
        case Command::UPDATE_PUB_PROFILE: handleUpdatePubProfileRequest(handler, fields);
            break;
        case Command::WITHDRAW_BALANCE: handleWithdrawBalanceRequest(handler, fields);
            break;

        case Command::DOWNLOAD_BOOK: handleDownloadBookRequest(handler, fields);
            break;
        case Command::UPDATE_LAST_PAGE: handleUpdateLastPageRequest(handler, fields);
            break;

        case Command::GET_COMMENTS_LIST: handleGetCommentsListRequest(handler, fields);
            break;
        case Command::GET_COMMENT_DETAILS: handleGetCommentDetailsRequest(handler, fields);
            break;
        case Command::UPDATE_COMMENT: handleUpdateCommentRequest(handler, fields);
            break;
        case Command::DELETE_COMMENT: handleDeleteCommentRequest(handler, fields);
            break;

        case Command::SEARCH_USERS: handleSearchUsersRequest(handler, fields);
            break;
        case Command::GET_USER_INFO: handleGetUserInfoRequest(handler, fields);
            break;
        case Command::DELETE_USER: handleDeleteUserRequest(handler, fields);
            break;
        case Command::BLOCK_USER: handleBlockUserRequest(handler, fields);
            break;
        case Command::UNBLOCK_USER: handleUnblockUserRequest(handler, fields);
            break;
        case Command::DELETE_COMMENT_ADMIN: handleDeleteCommentAdminRequest(handler, fields);
            break;

        case Command::GET_ALL_BOOKS_ADMIN: handleGetAllBooksAdminRequest(handler, fields);
            break;
        case Command::GET_BOOK_INFO_ADMIN: handleGetBookInfoAdminRequest(handler, fields);
            break;
        case Command::EDIT_BOOK_ADMIN: handleEditBookAdminRequest(handler, fields);
            break;
        case Command::GET_BOOK_COVER: handleGetBookCoverRequest(handler, fields);
            break;
        case Command::DOWNLOAD_PDF_ADMIN: handleDownloadPdfAdminRequest(handler, fields);
            break;

        case Command::GET_ALL_COMMENTS_ADMIN: handleGetAllCommentsAdminRequest(handler, fields);
            break;
        case Command::GET_COMMENT_INFO: handleGetCommentInfoRequest(handler, fields);
            break;

        case Command::GET_DATA_USERPROFILE: handleGetDataUserProfileRequest(handler, fields);
            break;
        case Command::GET_HISTORY_IDS: handleGetHistoryIdsRequest(handler, fields);
            break;
        case Command::GET_HISTORY_BOOK: handleGetHistoryBookRequest(handler, fields);
            break;
        case Command::UPDATE_BALANCE: handleUpdateBalanceRequest(handler, fields);
            break;
        case Command::UPDATE_PROFILE: handleUpdateProfileRequest(handler, fields);
            break;

        case Command::GET_NOTIFICATIONS: handleGetNotificationsRequest(handler, fields);
            break;
        case Command::GET_NOTIFICATION_INFO: handleGetNotificationInfoRequest(handler, fields);
            break;
        case Command::MARK_NOTIFICATION_READ: handleMarkNotificationReadRequest(handler, fields);
            break;
        case Command::MARK_ALL_NOTIFICATIONS_READ: handleMarkAllNotificationsReadRequest(handler, fields);
            break;


        default: handler->sendResponse(Command::FAIL, {"Not implemented yet"});
            break;
    }
}

User *ServerCore::findUserByUsername(const QString &username) {
    return data.findUserByUsername(username);
}

ClientHandler *ServerCore::findClientHandlerByUserId(quint64 userId) {
    dataMutex.lock();
    ClientHandler *handler = loggedInClients.value(userId, nullptr);
    dataMutex.unlock();
    return handler;
}

Role ServerCore::stringToRole(const QString &text, bool &ok) {
    ok = true;

    if (text == "ADMIN") {
        return Role::ADMIN;
    }
    if (text == "NORMALUSER") {
        return Role::USER;
    }
    if (text == "PUBLISHER") {
        return Role::PUBLISHER;
    }

    ok = false;
    return Role::USER;
}

quint64 ServerCore::requireAuthentication(ClientHandler *handler) {
    if (!handler->isAuthenticated()) {
        handler->sendResponse(Command::FAIL, {"Not authenticated"});
        return 0;
    }
    return handler->getUserId();
}

User *ServerCore::requireRole(ClientHandler *handler, Role requiredRole) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return nullptr;
    }

    User *user = data.getUsersMap().value(userId, nullptr);
    if (user == nullptr || user->getRole() != requiredRole) {
        handler->sendResponse(Command::FAIL, {"Not authorized"});
        return nullptr;
    }

    return user;
}

quint64 ServerCore::findOrCreateAuthorByName(const QString &authorName) {
    Author *author = data.findAuthorByName(authorName);
    if (author != nullptr) {
        return author->getId();
    }

    Author *newAuthor = new Author();
    newAuthor->assignNewId();
    newAuthor->setFullName(authorName);

    data.getAuthorsMap().insert(newAuthor->getId(), newAuthor);
    data.registerAuthorName(authorName, newAuthor->getId());

    return newAuthor->getId();
}

QString ServerCore::saveBase64File(const QString &base64Data, const QString &subDirectory,
                                   const QString &fileNameWithoutExt, const QString &extension) {
    if (base64Data.isEmpty()) {
        return "";
    }

    QByteArray rawBytes = QByteArray::fromBase64(base64Data.toUtf8());

    QString directoryPath = QCoreApplication::applicationDirPath() + "/assets/data/" + subDirectory + "/";

    QDir dir;
    dir.mkpath(directoryPath);

    QString filePath = directoryPath + fileNameWithoutExt + "." + extension;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return "";
    }
    file.write(rawBytes);
    file.close();

    return filePath;
}

QString ServerCore::readFileAsBase64(const QString &filePath) {
    if (filePath.isEmpty()) {
        return "";
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return "";
    }

    QByteArray rawBytes = file.readAll();
    file.close();

    return QString::fromUtf8(rawBytes.toBase64());
}

double ServerCore::averageStarsForBook(quint64 bookId) {
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        return 0.0;
    }

    QSet<quint64> reviewIds = book->getReviewIds();
    if (reviewIds.isEmpty()) {
        return 0.0;
    }

    int totalStars = 0;
    int count = 0;
    for (quint64 reviewId: reviewIds) {
        Review *review = data.getReviewsMap().value(reviewId, nullptr);
        if (review != nullptr) {
            totalStars += review->getStars();
            count++;
        }
    }

    if (count == 0) {
        return 0.0;
    }

    return (double) totalStars / (double) count;
}

bool ServerCore::canViewInactiveBook(ClientHandler *handler, Book *book) {
    if (!handler->isAuthenticated()) {
        return false;
    }

    quint64 userId = handler->getUserId();

    if (book->getPublisherId() == userId) {
        return true;
    }

    User *user = data.getUsersMap().value(userId, nullptr);
    if (user != nullptr && user->getRole() == Role::ADMIN) {
        return true;
    }

    return userHasPurchasedBook(userId, book->getId());
}

QString ServerCore::getCoverImageOrPlaceholder(const QString &coverImagePath) {
    QString imageBase64 = readFileAsBase64(coverImagePath);
    if (imageBase64.isEmpty()) {
        return "NO_IMAGE";
    }
    return imageBase64;
}

QString ServerCore::getAuthorName(quint64 authorId) {
    Author *author = data.getAuthorsMap().value(authorId, nullptr);
    if (author == nullptr) {
        return "";
    }
    return author->getFullName();
}

QString ServerCore::formatCommentId(quint64 reviewId) {
    return "C_" + QString::number(reviewId);
}

bool ServerCore::parseCommentId(const QString &text, quint64 &outId) {
    if (!text.startsWith("C_")) {
        return false;
    }

    bool ok = false;
    quint64 value = text.mid(2).toULongLong(&ok);
    if (!ok) {
        return false;
    }

    outId = value;
    return true;
}

Cart *ServerCore::findOrCreateCartForUser(quint64 userId) {
    Cart *cart = data.findCartByOwner(userId);
    if (cart != nullptr) {
        return cart;
    }

    Cart *newCart = new Cart();
    newCart->assignNewId();
    newCart->setOwnerId(userId);

    data.getCartsMap().insert(newCart->getId(), newCart);
    data.registerCartOwner(userId, newCart->getId());

    return newCart;
}

Library *ServerCore::getLibraryForUser(quint64 userId) {
    User *user = data.getUsersMap().value(userId, nullptr);
    if (user == nullptr || user->getRole() != Role::USER) {
        return nullptr;
    }

    NormalUser *normalUser = static_cast<NormalUser *>(user);
    return data.getLibrariesMap().value(normalUser->getLibraryId(), nullptr);
}

bool ServerCore::userHasPurchasedBook(quint64 userId, quint64 bookId) {
    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        return false;
    }
    return library->getPurchasedBooks().contains(bookId);
}

quint64 ServerCore::findShelfContainingBook(quint64 userId, quint64 bookId) {
    Library *library = getLibraryForUser(userId);
    if (library == nullptr) {
        return 0;
    }

    QSet<quint64> shelfIds = library->getShelves();
    for (quint64 shelfId: shelfIds) {
        Shelf *shelf = data.getShelvesMap().value(shelfId, nullptr);
        if (shelf != nullptr && shelf->getBookIds().contains(bookId)) {
            return shelfId;
        }
    }

    return 0;
}
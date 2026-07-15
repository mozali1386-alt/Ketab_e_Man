//in the name of ALLAH
//YA MAHDI

#ifndef SERVERCORE_H
#define SERVERCORE_H

#include <QObject>
#include <QVector>
#include <QSet>
#include <QMap>
#include <QMutex>
#include "network/ServerNetwork.h"
#include "network/ClientHandler.h"
#include "database/FileManager.h"
#include "logic/SearchEngine.h"
#include "ServerStats.h"
#include "ServerDataStore.h"
#include "../shared/User.h"
#include "../shared/NormalUser.h"
#include "../shared/Admin.h"
#include "../shared/Book.h"
#include "../shared/Author.h"
#include "../shared/Library.h"
#include "../shared/Shelf.h"
#include "../shared/Review.h"
#include "../shared/Cart.h"
#include "../shared/Notification.h"
#include "../shared/Enums.h"

class ServerCore : public QObject {
    Q_OBJECT

public:
    explicit ServerCore(QObject *parent = nullptr);

    virtual ~ServerCore();

    bool startServer(quint16 port);

    void stopServer();

    ServerDataStore &getDataStore();

    int getConnectedClientCount() const;

    int getTotalRequestCount() const;

signals:
    void logMessageGenerated(QString message);

    void connectedClientCountChanged(int count);

private slots:
    void onNewClientHandlerCreated(ClientHandler *handler);

    void onClientRequestReceived(ClientHandler *handler, int commandId, QString payload);

    void onClientDisconnected(ClientHandler *handler);

private:
    ServerNetwork *network;
    QSet<ClientHandler *> connectedClients;
    QMap<quint64, ClientHandler *> loggedInClients;
    mutable QMutex dataMutex;

    FileManager fileManager;
    SearchEngine searchEngine;
    ServerStats stats;
    ServerDataStore data;

    void handleRequest(ClientHandler *handler, int commandId, const QString &payload);

    void handleLoginRequest(ClientHandler *handler, const QString &payload);

    void handleRegisterRequest(ClientHandler *handler, const QString &payload);

    void handleForgotPasswordRequest(ClientHandler *handler, const QString &payload);

    void handleResetPasswordRequest(ClientHandler *handler, const QString &payload);

    void handleLogoutRequest(ClientHandler *handler, const QString &payload);

    void handlePublishRequest(ClientHandler *handler, const QString &payload);

    void handleUpdateBookRequest(ClientHandler *handler, const QString &payload);

    void handleDeactivateBookRequest(ClientHandler *handler, const QString &payload);

    void handleBuyRequest(ClientHandler *handler, const QString &payload);

    void handleSearchRequest(ClientHandler *handler, const QString &payload);

    void handleAddReviewRequest(ClientHandler *handler, const QString &payload);

    void handleEditReviewRequest(ClientHandler *handler, const QString &payload);

    void handleDeleteReviewRequest(ClientHandler *handler, const QString &payload);

    void handleAddToCartRequest(ClientHandler *handler, const QString &payload);

    void handleRemoveFromCartRequest(ClientHandler *handler, const QString &payload);

    void handleCheckoutCartRequest(ClientHandler *handler, const QString &payload);

    void handleCreateShelfRequest(ClientHandler *handler, const QString &payload);

    void handleAddBookToShelfRequest(ClientHandler *handler, const QString &payload);

    void handleSaveBookRequest(ClientHandler *handler, const QString &payload);

    void handleBlockUserRequest(ClientHandler *handler, const QString &payload);

    void handleUnblockUserRequest(ClientHandler *handler, const QString &payload);

    void handleDeleteBookRequest(ClientHandler *handler, const QString &payload);

    void handleSetFavoriteGenresRequest(ClientHandler *handler, const QString &payload);

    User *findUserByUsername(const QString &username);

    Cart *findOrCreateCartForUser(quint64 userId);

    ClientHandler *findClientHandlerByUserId(quint64 userId);

    bool splitPayloadOrFail(ClientHandler *handler, const QString &payload, int minParts, QStringList &outParts);

    bool parseGenre(const QString &text, Genre &out);

    bool parseDouble(const QString &text, double &out);

    quint64 requireAuthentication(ClientHandler *handler);

    Admin *requireAdmin(quint64 userId, ClientHandler *handler);

    NormalUser *requireNormalUser(quint64 userId, ClientHandler *handler);

    Library *requireUserLibrary(quint64 userId, ClientHandler *handler);

    Book *requireOwnedBook(quint64 bookId, quint64 userId, ClientHandler *handler);

    void sendNotificationToUser(quint64 userId, NotificationType type, const QString &message);

    void notifyUsersAboutNewBook(Book *book);

    void broadcastReviewUpdated(quint64 bookId);

    void purgeBookFromNonOwnerPlaces(quint64 bookId);

    bool libraryOwnerHasPurchasedBook(quint64 ownerId, quint64 bookId);

    bool processPurchase(ClientHandler *handler, quint64 userId, const QVector<quint64> &bookIdList);

    void deleteAllReviewsForBook(quint64 bookId, Book *book);
};

#endif

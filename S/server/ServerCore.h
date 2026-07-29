//in the name of ALLAH
//YA MAHDI

#ifndef SERVERCORE_H
#define SERVERCORE_H

#include <QObject>
#include <QSet>
#include <QMap>
#include <QMutex>
#include <QStringList>
#include <QVector>
#include "network/ServerNetwork.h"
#include "network/ClientHandler.h"
#include "database/FileManager.h"
#include "ServerStats.h"
#include "ServerDataStore.h"
#include "../shared/User.h"
#include "../shared/NormalUser.h"
#include "../shared/Publisher.h"
#include "../shared/Enums.h"
#include "../shared/Protocol.h"


struct PdfUploadState {
    QStringList chunks;
    int totalChunks;
    quint64 uploaderId;
};


struct PurchaseOutcome {
    bool success;
    QString failReason;
    quint64 purchaseId;
    double totalAmount;
};

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

    void onClientRequestReceived(ClientHandler *handler, Command command, QStringList fields);

    void onClientDisconnected(ClientHandler *handler);

private:
    ServerNetwork *network;
    QSet<ClientHandler *> connectedClients;
    QMap<quint64, ClientHandler *> loggedInClients;
    mutable QMutex dataMutex;

    FileManager fileManager;
    ServerStats stats;
    ServerDataStore data;

    QMap<quint64, PdfUploadState> pdfUploads;

    void handleRequest(ClientHandler *handler, Command command, const QStringList &fields);

    void ensureDefaultAdminExists();

    void handleLoginRequest(ClientHandler *handler, const QStringList &fields);

    void handleSignupPublisherRequest(ClientHandler *handler, const QStringList &fields);

    void handleSignupNormalUserRequest(ClientHandler *handler, const QStringList &fields);

    void handleLogoutRequest(ClientHandler *handler, const QStringList &fields);

    void handleForgotPassCheckRequest(ClientHandler *handler, const QStringList &fields);

    void handleForgotPassUpdateRequest(ClientHandler *handler, const QStringList &fields);


    void handleGetPublisherBooksRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetPubBookDetailsRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetBookDetailsRequest(ClientHandler *handler, const QStringList &fields);

    void handleAddBookMetadataRequest(ClientHandler *handler, const QStringList &fields);

    void handleUploadPdfStartRequest(ClientHandler *handler, const QStringList &fields);

    void handleUploadPdfChunkRequest(ClientHandler *handler, const QStringList &fields);

    void handleUploadPdfEndRequest(ClientHandler *handler, const QStringList &fields);

    void handleEditBookRequest(ClientHandler *handler, const QStringList &fields);

    void handleToggleBookStatusRequest(ClientHandler *handler, const QStringList &fields);

    void handleDeleteBookRequest(ClientHandler *handler, const QStringList &fields);

    void handleSearchStorepageRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetBookSummaryRequest(ClientHandler *handler, const QStringList &fields);


    void handleBuyRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetCartItemsRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetCartBookSummaryRequest(ClientHandler *handler, const QStringList &fields);

    void handleAddCartRequest(ClientHandler *handler, const QStringList &fields);

    void handleRemoveCartRequest(ClientHandler *handler, const QStringList &fields);

    void handleCheckoutCartRequest(ClientHandler *handler, const QStringList &fields);

    Cart *findOrCreateCartForUser(quint64 userId);

    PurchaseOutcome performPurchase(quint64 buyerId, const QSet<quint64> &bookIds);


    void handleGetMyBooksRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetSavedBooksRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetShelvesRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetShelfBooksRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetMyBookInfoRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetSavedBookInfoRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetShelfBookInfoRequest(ClientHandler *handler, const QStringList &fields);

    void handleAddShelfRequest(ClientHandler *handler, const QStringList &fields);

    void handleEditShelfRequest(ClientHandler *handler, const QStringList &fields);

    void handleDeleteShelfRequest(ClientHandler *handler, const QStringList &fields);

    void handleUnsaveBookRequest(ClientHandler *handler, const QStringList &fields);

    void handleRemoveFromShelfRequest(ClientHandler *handler, const QStringList &fields);

    void handleAssignToShelfRequest(ClientHandler *handler, const QStringList &fields);

    void handleSaveBookRequest(ClientHandler *handler, const QStringList &fields);

    Library *getLibraryForUser(quint64 userId);

    bool userHasPurchasedBook(quint64 userId, quint64 bookId);

    quint64 findShelfContainingBook(quint64 userId, quint64 bookId);


    void handleGetPubGeneralStatsRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetPubTopBooksIdsRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetPubBookInfoTopRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetPubLowestBooksIdsRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetPubBookInfoLowestRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetPubSalesChartRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetPubScoreChartRequest(ClientHandler *handler, const QStringList &fields);

    QVector<quint64> sortedPublisherBookIdsBySales(Publisher *publisher, bool descending);

    void handleGetPubProfileInfoRequest(ClientHandler *handler, const QStringList &fields);

    void handleUpdatePubProfileRequest(ClientHandler *handler, const QStringList &fields);

    void handleWithdrawBalanceRequest(ClientHandler *handler, const QStringList &fields);


    void handleDownloadBookRequest(ClientHandler *handler, const QStringList &fields);

    void handleUpdateLastPageRequest(ClientHandler *handler, const QStringList &fields);


    void handleGetCommentsListRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetCommentDetailsRequest(ClientHandler *handler, const QStringList &fields);

    void handleUpdateCommentRequest(ClientHandler *handler, const QStringList &fields);

    void handleDeleteCommentRequest(ClientHandler *handler, const QStringList &fields);

    QString formatCommentId(quint64 reviewId);

    bool parseCommentId(const QString &text, quint64 &outId);


    void handleSearchUsersRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetUserInfoRequest(ClientHandler *handler, const QStringList &fields);

    void handleDeleteUserRequest(ClientHandler *handler, const QStringList &fields);

    void handleBlockUserRequest(ClientHandler *handler, const QStringList &fields);

    void handleUnblockUserRequest(ClientHandler *handler, const QStringList &fields);

    void handleDeleteCommentAdminRequest(ClientHandler *handler, const QStringList &fields);


    void handleGetAllBooksAdminRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetBookInfoAdminRequest(ClientHandler *handler, const QStringList &fields);

    void handleEditBookAdminRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetBookCoverRequest(ClientHandler *handler, const QStringList &fields);

    void handleDownloadPdfAdminRequest(ClientHandler *handler, const QStringList &fields);


    void handleGetAllCommentsAdminRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetCommentInfoRequest(ClientHandler *handler, const QStringList &fields);


    void cascadeDeleteBook(quint64 bookId);

    void cascadeDeleteReviewsByAuthor(quint64 userId);

    void cascadeDeleteWalletAndTransactions(quint64 walletId);

    void cascadeDeleteNotificationsFor(quint64 userId);


    void handleGetDataUserProfileRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetHistoryIdsRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetHistoryBookRequest(ClientHandler *handler, const QStringList &fields);

    void handleUpdateBalanceRequest(ClientHandler *handler, const QStringList &fields);

    void handleUpdateProfileRequest(ClientHandler *handler, const QStringList &fields);


    void pushNotification(quint64 recipientUserId, NotificationType type, const QString &message);

    void pushAdminStatsUpdate();

    void handleGetNotificationsRequest(ClientHandler *handler, const QStringList &fields);

    void handleGetNotificationInfoRequest(ClientHandler *handler, const QStringList &fields);

    void handleMarkNotificationReadRequest(ClientHandler *handler, const QStringList &fields);

    void handleMarkAllNotificationsReadRequest(ClientHandler *handler, const QStringList &fields);

    User *findUserByUsername(const QString &username);

    ClientHandler *findClientHandlerByUserId(quint64 userId);

    Role stringToRole(const QString &text, bool &ok);

    quint64 requireAuthentication(ClientHandler *handler);

    User *requireRole(ClientHandler *handler, Role requiredRole);

    quint64 findOrCreateAuthorByName(const QString &authorName);

    QString saveBase64File(const QString &base64Data, const QString &subDirectory, const QString &fileNameWithoutExt,
                           const QString &extension);

    QString readFileAsBase64(const QString &filePath);

    double averageStarsForBook(quint64 bookId);


    bool canViewInactiveBook(ClientHandler *handler, Book *book);


    QString getCoverImageOrPlaceholder(const QString &coverImagePath);

    QString getAuthorName(quint64 authorId);
};

#endif
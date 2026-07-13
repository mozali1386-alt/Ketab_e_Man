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
#include "../shared/User.h"
#include "../shared/Book.h"
#include "../shared/Author.h"
#include "../shared/Library.h"
#include "../shared/Shelf.h"
#include "../shared/Review.h"
#include "../shared/Cart.h"
#include "../shared/Notification.h"
#include "../shared/Wallet.h"
#include "../shared/Transaction.h"
#include "../shared/Purchase.h"
#include "../shared/Enums.h"

class ServerCore : public QObject {
    Q_OBJECT

public:
    explicit ServerCore(QObject *parent = nullptr);

    virtual ~ServerCore();

    bool startServer(quint16 port);

    void stopServer();

    QMap<quint64, User *> &getUsersMap();

    QMap<quint64, Book *> &getBooksMap();

    QMap<quint64, Author *> &getAuthorsMap();

    QMap<quint64, Library *> &getLibrariesMap();

    QMap<quint64, Shelf *> &getShelvesMap();

    QMap<quint64, Review *> &getReviewsMap();

    QMap<quint64, Cart *> &getCartsMap();

    QMap<quint64, Notification *> &getNotificationsMap();

    QMap<quint64, Wallet *> &getWalletsMap();

    QMap<quint64, Transaction *> &getTransactionsMap();

    QMap<quint64, Purchase *> &getPurchasesMap();

    int getConnectedClientCount() const;

    int getTotalRequestCount() const;

signals:
    void logMessageGenerated(QString message);

    void connectedClientCountChanged(int count);

private slots:
    void onNewClientHandlerCreated(ClientHandler *handler);

    void onClientRequestReceived(ClientHandler *handler, int commandId, QString payload);

    void onClientDisconnected(ClientHandler *handler);

};

#endif

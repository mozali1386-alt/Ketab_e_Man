//in the name of ALLAH
//YA MAHDI

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include "../../shared/Protocol.h"

class ClientHandler : public QObject {
    Q_OBJECT

public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);

    virtual ~ClientHandler();


    void sendResponse(Command command, const QStringList &fields = QStringList());

    quint64 getUserId() const;

    void setUserId(quint64 newUserId);

    bool isAuthenticated() const;


    quint64 getRecoveryUserId() const;

    void setRecoveryUserId(quint64 newRecoveryUserId);

signals:
    void requestReceived(ClientHandler *handler, Command command, QStringList fields);

    void clientDisconnected(ClientHandler *handler);

private slots:
    void onReadyRead();

    void onDisconnected();

    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    quint32 blockSize;
    quint64 userId;
    quint64 recoveryUserId;
    static const quint32 MAX_BLOCK_SIZE = 10485760;

    QString buildMessage(Command command, const QStringList &fields) const;
};

#endif

//in the name of ALLAH
//YA MAHDI

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QString>

class ClientHandler : public QObject {
    Q_OBJECT

public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);

    virtual ~ClientHandler();

    void sendResponse(int commandId, const QString &payload);

    quint64 getUserId() const;

    void setUserId(quint64 newUserId);

    bool isAuthenticated() const;

signals:
    void requestReceived(ClientHandler *handler, int commandId, QString payload);

    void clientDisconnected(ClientHandler *handler);

private slots:
    void onReadyRead();

    void onDisconnected();

    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    quint32 blockSize;
    quint64 userId;
    static const quint32 MAX_BLOCK_SIZE = 10485760;
};

#endif

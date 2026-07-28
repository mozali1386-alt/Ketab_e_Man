#ifndef CLIENTSOCKETMANAGER_H
#define CLIENTSOCKETMANAGER_H

#include <QObject>
#include <QString>
#include <QTcpSocket>

class ClientSocketManager : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocketManager(QObject *parent = nullptr);
    ~ClientSocketManager();

    void connectToServer(const QString &hostAddress, quint16 port);
    void sendMessage(const QString &message);

signals:
    void messageReceived(const QString &message);
    void connectedToServer();
    void disconnectedFromServer();
    void connectionErrorOccurred(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    quint32 blockSize;
};

#endif
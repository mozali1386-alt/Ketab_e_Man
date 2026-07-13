//in the name of ALLAH
//YA MAHDI

#include "ClientHandler.h"
#include <QDataStream>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent) : QObject(parent) {
    blockSize = 0;
    userId = 0;

    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
}

ClientHandler::~ClientHandler() {
    if (socket != nullptr) {
        socket->deleteLater();
        socket = nullptr;
    }
}

void ClientHandler::onReadyRead() {
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_10);

    while (true) {
        if (blockSize == 0) {
            if (socket->bytesAvailable() < (qint64) sizeof(quint32)) {
                return;
            }
            in >> blockSize;
        }

        if (socket->bytesAvailable() < (qint64) blockSize) {
            return;
        }

        int commandId = 0;
        QString payload = "";
        in >> commandId;
        in >> payload;

        blockSize = 0;

        emit requestReceived(this, commandId, payload);
    }
}

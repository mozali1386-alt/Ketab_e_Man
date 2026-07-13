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

void ClientHandler::onDisconnected() {
    emit clientDisconnected(this);
}

void ClientHandler::sendResponse(int commandId, const QString &payload) {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    out << (quint32) 0;
    out << commandId;
    out << payload;

    out.device()->seek(0);
    out << (quint32) (block.size() - (int) sizeof(quint32));

    socket->write(block);
}

quint64 ClientHandler::getUserId() const {
    return userId;
}

void ClientHandler::setUserId(quint64 newUserId) {
    userId = newUserId;
}

bool ClientHandler::isAuthenticated() const {
    if (userId == 0) {
        return false;
    }
    return true;
}

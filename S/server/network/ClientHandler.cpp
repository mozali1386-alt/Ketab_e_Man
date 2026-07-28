//in the name of ALLAH
//YA MAHDI

#include "ClientHandler.h"
#include <QDataStream>

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent) : QObject(parent) {
    blockSize = 0;
    userId = 0;
    recoveryUserId = 0;

    socket = new QTcpSocket(this);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        deleteLater();
        return;
    }

    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientHandler::onSocketError);
}

ClientHandler::~ClientHandler() {
}

QString ClientHandler::buildMessage(Command command, const QStringList &fields) const {
    QString message = commandToString(command);
    for (int i = 0; i < fields.size(); i++) {
        message += PROTOCOL_SEPARATOR + fields.at(i);
    }
    return message;
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

            if (blockSize > MAX_BLOCK_SIZE) {
                socket->disconnectFromHost();
                return;
            }
        }

        if (socket->bytesAvailable() < (qint64) blockSize) {
            return;
        }

        QString message = "";
        in >> message;

        if (in.status() != QDataStream::Ok) {
            socket->disconnectFromHost();
            return;
        }

        blockSize = 0;

        QStringList parts = message.split(PROTOCOL_SEPARATOR);
        Command command = stringToCommand(parts.at(0));
        parts.removeFirst();

        emit requestReceived(this, command, parts);
    }
}

void ClientHandler::onDisconnected() {
    emit clientDisconnected(this);
}

void ClientHandler::onSocketError(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError);
    socket->abort();
}

void ClientHandler::sendResponse(Command command, const QStringList &fields) {
    if (socket == nullptr || socket->state() != QAbstractSocket::ConnectedState) {
        return;
    }

    QString message = buildMessage(command, fields);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    out << (quint32) 0;
    out << message;

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

quint64 ClientHandler::getRecoveryUserId() const {
    return recoveryUserId;
}

void ClientHandler::setRecoveryUserId(quint64 newRecoveryUserId) {
    recoveryUserId = newRecoveryUserId;
}

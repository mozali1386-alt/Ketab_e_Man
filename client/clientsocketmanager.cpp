#include "clientsocketmanager.h"
#include <QDataStream>
#include <QDebug>

ClientSocketManager::ClientSocketManager(QObject *parent)
    : QObject(parent)
{
    blockSize = 0;
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &ClientSocketManager::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &ClientSocketManager::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &ClientSocketManager::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientSocketManager::onErrorOccurred);
}

ClientSocketManager::~ClientSocketManager()
{
    if (socket != nullptr) {
        socket->deleteLater();
        socket = nullptr;
    }
}

void ClientSocketManager::connectToServer(const QString &hostAddress, quint16 port)
{
    socket->connectToHost(hostAddress, port);
}

void ClientSocketManager::sendMessage(const QString &message)
{
    qDebug() << "client sent:   " << message;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // هماهنگ‌سازی نسخه با سرورِ هم‌تیمی‌ات
    out.setVersion(QDataStream::Qt_5_10);

    //  نوشتن یک عدد صفر به صورت موقت تا جا برای سایز پیام باز شود
    out << (quint32) 0;

    //  نوشتن دقیقاً همان رشته متنی که تو از داشبورد فرستاده‌ای (مثلاً "LOGIN||ali||123")
    out << message;

    out.device()->seek(0);
    out << (quint32) (block.size() - (int) sizeof(quint32));
    socket->write(block);
}

void ClientSocketManager::onConnected()
{
    //emit connectedToServer();
    qDebug() << "successfuly connected to the server";
}

void ClientSocketManager::onDisconnected()
{
    //emit disconnectedFromServer();
    qDebug() << "disconnected from the server";
}

void ClientSocketManager::onReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_10);

    while (true) {
        // ۱. خواندن سایز پیام
        if (blockSize == 0) {
            if (socket->bytesAvailable() < (qint64) sizeof(quint32)) {
                return; // کل سایز هنوز نرسیده، منتظر می‌مانیم
            }
            in >> blockSize;
        }

        if (socket->bytesAvailable() < (qint64) blockSize) {
            return; // هنوز بخش‌هایی از پیام تو راه است، منتظر می‌مانیم
        }

        QString message = "";
        in >> message;

        qDebug() << "server replied:   " << message;

        // ۴. صفر کردن مجدد سایز برای پیام‌های بعدی که در راه هستند
        blockSize = 0;

        //  ارسال دقیقِ رشته به سمت کدهای تو (دقیقاً همان چیزی که کدهایت منتظرش هستند
        emit messageReceived(message);
    }
}

void ClientSocketManager::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    //emit connectionErrorOccurred(socket->errorString());
    qDebug() << socket->errorString();
}
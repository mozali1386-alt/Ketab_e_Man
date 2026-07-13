//in the name of ALLAH
//YA MAHDI

#include "ServerNetwork.h"

ServerNetwork::ServerNetwork(QObject *parent) : QTcpServer(parent) {
}

ServerNetwork::~ServerNetwork() {
}

bool ServerNetwork::startListening(quint16 port) {
    bool success = this->listen(QHostAddress::Any, port);
    return success;
}

void ServerNetwork::stopListening() {
    this->close();
}

void ServerNetwork::incomingConnection(qintptr socketDescriptor) {
    ClientHandler *handler = new ClientHandler(socketDescriptor, this);
    emit newClientHandlerCreated(handler);
}

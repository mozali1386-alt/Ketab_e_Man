//in the name of ALLAH
//YA MAHDI

#ifndef SERVERNETWORK_H
#define SERVERNETWORK_H

#include <QTcpServer>
#include "ClientHandler.h"

class ServerNetwork : public QTcpServer {
    Q_OBJECT

public:
    explicit ServerNetwork(QObject *parent = nullptr);

    virtual ~ServerNetwork();

    bool startListening(quint16 port);

    void stopListening();

signals:
    void newClientHandlerCreated(ClientHandler *handler);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif

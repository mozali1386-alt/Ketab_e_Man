//in the name of ALLAH
//YA MAHDI

#ifndef SERVERSTATS_H
#define SERVERSTATS_H

class ServerStats {
public:
    ServerStats();

    int getConnectedUsers() const;

    void setConnectedUsers(int value);

    int getTotalRequests() const;

    void incrementTotalRequests();

private:
    int connectedUsers;
    int totalRequests;
};

#endif

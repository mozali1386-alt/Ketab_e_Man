//in the name of ALLAH
//YA MAHDI

#include "ServerStats.h"

ServerStats::ServerStats() {
    connectedUsers = 0;
    totalRequests = 0;
}

int ServerStats::getConnectedUsers() const {
    return connectedUsers;
}

void ServerStats::setConnectedUsers(int value) {
    connectedUsers = value;
}

int ServerStats::getTotalRequests() const {
    return totalRequests;
}

void ServerStats::incrementTotalRequests() {
    totalRequests = totalRequests + 1;
}

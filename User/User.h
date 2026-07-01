//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_USER_H
#define KETAB_E_MAN_USER_H
#include <cstdint>
#include <string>

#include "../loginwindow.h"


class User {
    static long long usersCounter;
    uint64_t userId;
    std::string userName;
    std::string passwordHash;
    std::string email;
    QDateTime registerationDate;
    static enum UserStatus{
        ACTIVE, BLOCKED
    };
    UserStatus userStatus;
    static uint64_t generateUserId();
    bool checkPassword(const std::string & password) const;
public:
    User(const std::string & userName, const std::string & password,const std::string & email);
    ~User();
    static bool isValidEmail(const std::string & email);
    static bool isValidPassword(const std::string & password);
};

#endif //KETAB_E_MAN_USER_H

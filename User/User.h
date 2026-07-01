//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_USER_H
#define KETAB_E_MAN_USER_H
#include <cstdint>
#include <string>

#include "../loginwindow.h"


class User {
    static long long usersCounter;
protected:
    uint64_t userId;
    std::string userName;
    std::string passwordHash;
    std::string email;
    //QDateTime registerationDate;
    enum UserStatus{
        ACTIVE, BLOCKED
    };
    UserStatus userStatus;
private:
    virtual uint64_t generateUserId() = 0;

public:
    bool checkPassword(const std::string & password) const;

    User(const std::string & userName, const std::string & password,const std::string & email);
    ~User();
    static bool isValidEmail(const std::string & email);
    static bool isValidPassword(const std::string & password);
};

#endif //KETAB_E_MAN_USER_H

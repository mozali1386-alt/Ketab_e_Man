//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_USER_H
#define KETAB_E_MAN_USER_H
#include <cstdint>
#include <string>
#include <QDateTime>
#include "../loginwindow.h"


class User {
protected:
    static long long usersCounter;
    uint64_t userId{0};
    std::string userName;
    std::string passwordHash;
    std::string email;
    QDateTime registerationDate;
    enum UserStatus{
        ACTIVE, BLOCKED
    };
    UserStatus userStatus;
private:
    virtual uint64_t generateUserId() = 0;
    bool checkPassword(const std::string & password) const;
public:
    User(const std::string & userName, const std::string & email, const std::string & password);
    ~User();
    void changePassword(const std::string & password);
    static bool isValidEmail(const std::string & email);
    static bool isValidPassword(const std::string & password);

};

#endif //KETAB_E_MAN_USER_H

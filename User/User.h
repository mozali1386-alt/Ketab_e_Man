//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_USER_H
#define KETAB_E_MAN_USER_H
#include <cstdint>
#include <string>
#include <QDateTime>
#include "../loginwindow.h"


class User {
public:
    enum UserStatus{
        ACTIVE, BLOCKED
    };
private:
    std::string userName;
    std::string passwordHash;
    std::string email;
    QDateTime registerationDate;
    UserStatus userStatus;
protected:
    uint64_t userId{0};
    static long long usersCounter;

private:
    virtual uint64_t generateUserId() = 0;
    bool checkPassword(const std::string & password) const;
public:
    User(const std::string & userName, const std::string & email, const std::string & password);
    virtual ~User();
    void changePassword(const std::string & password);
    static bool isValidEmail(const std::string & email);
    static bool isValidPassword(const std::string & password);
    std::string userNameGetter() const;

    std::string passwordHashGetter() const;

    std::string emailGetter() const;

    QDateTime registerationDateGetter() const;

    UserStatus userStatusGetter() const;

    uint64_t userIdGetter() const;

};

#endif //KETAB_E_MAN_USER_H

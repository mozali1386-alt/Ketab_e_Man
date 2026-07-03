//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_USER_H
#define KETAB_E_MAN_USER_H

#include <string>
#include <QDateTime>
#include "../loginwindow.h"

class User{
protected:
    static uint64_t usersCounter;
    uint64_t userId;
    std::string userName;
    std::string passwordHash;
    std::string email;
    QDateTime registerationDate;

public:
    enum UserStatus{
        ACTIVE,
        BLOCKED
    };

protected:
    UserStatus status;
    virtual uint64_t generateUserId() = 0;

public:
    User(const std::string& userName,
         const std::string& password,
         const std::string& email);
    virtual ~User();
    bool checkPassword(const std::string& password) const;
    bool changePassword(const std::string& oldPassword,
                        const std::string& newPassword);
    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);
    uint64_t getUserId() const;
    std::string getUserName() const;
    std::string getEmail() const;
    QDateTime getRegisterationDate() const;
    UserStatus getStatus() const;
    bool isBlocked() const;
    void setUserName(const std::string& userName);
    void setEmail(const std::string& email);
    void block();
    void unblock();
};

#endif //KETAB_E_MAN_USER_H

//in the name of ALLAH
//YA MAHDI

#include <QDateTime>
#include "User.h"
#include "../additional library/libbcrypt/include/bcrypt/BCrypt.hpp"

long long User::usersCounter = 0ll;

bool User::checkPassword(const std::string &password) const {
    return BCrypt::validatePassword(password, passwordHash);
}

User::User(const std::string & userName, const std::string & password,const std::string & email) {
    usersCounter++;
    this->userName = userName;
    this->passwordHash = BCrypt::generateHash(password, 12);
    this->email = email;
    registerationDate = QDateTime(QDateTime::currentDateTime());
    userStatus = ACTIVE;
}
User::~User() {
}

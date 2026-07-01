//in the name of ALLAH
//YA MAHDI


#include <QDateTime>
#include "User.h"

#include "../additional library/libbcrypt/include/bcrypt/BCrypt.hpp"


uint64_t User::generateUserId() {
}

bool User::checkPassword(const std::string &password) {
    return BCrypt::validatePassword(password, passwordHash);
}

User::User(const std::string & userName, const std::string & password,const std::string & email) {
    this->userName = userName;
    this->passwordHash = BCrypt::generateHash(password, 12);
    this->email = email;
    registerationDate = new QDateTime(QDateTime::currentDateTime());
}
User::~User() {
    delete registerationDate;
}

//in the name of ALLAH
//YA MAHDI

#include <QDateTime>
#include "User.h"
#include "../additional library/libbcrypt/include/bcrypt/BCrypt.hpp"
#include <regex>


long long User::usersCounter = 0ll;

bool User::checkPassword(const std::string &password) const {
    return BCrypt::validatePassword(password, passwordHash);
}

User::User(const std::string & userName, const std::string & password,const std::string & email) {
    usersCounter++;
    this->userName = userName;
    this->passwordHash = BCrypt::generateHash(password, 12);
    this->email = email;
    //registerationDate = QDateTime(QDateTime::currentDateTime());
    userStatus = ACTIVE;
}
User::~User() {
}

bool User::isValidEmail(const std::string &email) {
    const std::regex pattern(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );
    return std::regex_match(email, pattern);
}

bool User::isValidPassword(const std::string &password) {
    const std::regex pattern(
        R"((?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{8,})"
    );
    return std::regex_match(password, pattern);
}


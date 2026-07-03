//in the name of ALLAH
//YA MAHDI

#include "User.h"
#include "../additional library/libbcrypt/include/bcrypt/BCrypt.hpp"
#include <regex>

uint64_t User::usersCounter = 0;

User::User(const std::string &userName,
           const std::string &password,
           const std::string &email) {
    usersCounter++;
    this->userName = userName;
    this->email = email;
    passwordHash = BCrypt::generateHash(password, 12);
    registerationDate = QDateTime::currentDateTime();
    status = ACTIVE;
}

User::~User() {
}

bool User::checkPassword(const std::string &password) const {
    return BCrypt::validatePassword(password, passwordHash);
}

bool User::changePassword(const std::string &oldPassword,
                          const std::string &newPassword) {
    if (!checkPassword(oldPassword))
        return false;
    if (!isValidPassword(newPassword))
        return false;
    passwordHash = BCrypt::generateHash(newPassword, 12);
    return true;
}

bool User::isValidEmail(const std::string &email) {
    const std::regex pattern(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );
    return std::regex_match(email, pattern);
}

bool User::isValidPassword(const std::string &password) {
    const std::regex pattern(
        R"((?=.*[a-z])(?=.*[A-Z])(?=.*\d).{8,})"
    );
    return std::regex_match(password, pattern);
}

uint64_t User::getUserId() const {
    return userId;
}

std::string User::getUserName() const {
    return userName;
}

std::string User::getEmail() const {
    return email;
}

QDateTime User::getRegisterationDate() const {
    return registerationDate;
}

User::UserStatus User::getStatus() const {
    return status;
}

bool User::isBlocked() const {
    return status == BLOCKED;
}

void User::setUserName(const std::string &userName) {
    this->userName = userName;
}

void User::setEmail(const std::string &email) {
    this->email = email;
}

void User::block() {
    status = BLOCKED;
}

void User::unblock() {
    status = ACTIVE;
}
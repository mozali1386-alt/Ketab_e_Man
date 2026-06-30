//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_USER_H
#define KETAB_E_MAN_USER_H
#include <cstdint>
#include <string>

#include "../loginwindow.h"


class User {
    uint64_t userId;
    std::string userName;
    std::uint64_t passwordHash;
    std::string email;
    QDateTime* registerationDate;

public:
    User();
    ~User();
};

#endif //KETAB_E_MAN_USER_H

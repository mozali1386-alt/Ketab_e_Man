//in the name of ALLAH
//YA MAHDI

#include "NormalUser.h"

uint64_t NormalUser::generateUserId() {
    this->userId = 0;
    //most edit
}

NormalUser::NormalUser(const std::string name, const std::string lastName,
    const std::string username, const std::string password, const std::string email)
    :User(username, password, email) {
    this->name = name;
    this->lastName = lastName;
}

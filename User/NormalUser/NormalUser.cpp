//
// Created by Mohammad Habib on 6/30/2026.
//

#include "NormalUser.h"

uint64_t NormalUser::generateUserId() {
    this->userId = 0;
    //most edit
}

NormalUser::NormalUser(const std::string username, const std::string password, const std::string email) :User(username, password, email) {
}

//
// Created by Mohammad Habib on 6/30/2026.
//

#include "NormalUser.h"

uint64_t NormalUser::generateUserId() {
    this->userId = 0;
    //most edit
}

NormalUser::NormalUser(std::string username, std::string password, std::string email, double walletBalance) :User(username, password, email) {
}

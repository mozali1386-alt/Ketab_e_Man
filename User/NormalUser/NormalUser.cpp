//in the name of ALLAH
//YA MAHDI

#include "NormalUser.h"



uint64_t NormalUser::generateUserId() {
    if (!userId) {
        uint64_t value = static_cast<uint64_t>(qHash(this->getUserName()));
        uint64_t tenDigit = value % 10000000000ULL;
        uint64_t newUserId = 3ull * 1000000 * 1000000 * 1000000 + tenDigit * 10000 * 10000 + ++usersCounter;
        return newUserId;
    }
    return this->userId;
}

NormalUser::NormalUser(const std::string & name, const std::string & lastName,
    const std::string &username, const std::string & email, const std::string & password)
    :User(username, email, password) {
    userId = NormalUser::generateUserId();
    this->name = name;
    this->lastName = lastName;
}
std::string NormalUser::getName() const {
    return name;
}

std::string NormalUser::getLastName() const {
    return lastName;
}

double NormalUser::getWalletBalance() const {
    return walletBalance;
}

uint64_t NormalUser::getPersonalLibraryId() const {
    return personalLibraryId;
}

uint64_t NormalUser::getActiveCartId() const {
    return activeCartId;
}

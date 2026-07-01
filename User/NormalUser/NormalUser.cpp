//in the name of ALLAH
//YA MAHDI

#include "NormalUser.h"

uint64_t NormalUser::generateUserId() {
    if (!userId) {
        uint64_t value = static_cast<uint64_t>(qHash(userName));
        uint64_t tenDigit = value % 10000000000ULL;
        uint64_t newUserId = 2ull * 1000000 * 1000000 * 1000000 + tenDigit * 10000 * 10000 + ++usersCounter;
        return newUserId;
    }
    return this->userId;
}

NormalUser::NormalUser(const std::string name, const std::string lastName,
    const std::string username, const std::string password, const std::string email)
    :User(username, password, email) {
    userId = NormalUser::generateUserId();
    this->name = name;
    this->lastName = lastName;
}

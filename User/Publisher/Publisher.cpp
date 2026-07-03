//in the name of ALLAH
//YA MAHDI

#include "Publisher.h"

uint64_t Publisher::generateUserId() {
    if (!id) {
        uint64_t value = static_cast<uint64_t>(qHash(this->getUserName()));
        uint64_t tenDigit = value % 10000000000ULL;
        uint64_t newUserId = 11ull * 1000000 * 1000000 * 1000000 + tenDigit * 10000 * 10000 + ++usersCounter;
        return newUserId;
    }
    return this->id;
}

Publisher::Publisher(const std::string &name, const std::string &userName, const std::string &email,
                     const std::string &password) :User(userName, email, password){
    this->name = name;
    id = Publisher::generateUserId();
}

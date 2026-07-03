//in the name of ALLAH
//YA MAHDI

#include "Admin.h"

Admin::Admin(const std::string & userName, const std::string & email, const std::string & password) :User(userName, email, password){
    this->id = Admin::generateUserId();
}

uint64_t Admin::generateUserId() {
    if (!id) {
        uint64_t value = static_cast<uint64_t>(qHash(this->getUserName()));
        uint64_t tenDigit = value % 10000000000ULL;
        uint64_t newUserId = 10ull * 1000000 * 1000000 * 1000000 + tenDigit * 10000 * 10000 + ++usersCounter;
        return newUserId;
    }
    return this->id;
}

Admin & Admin::getAdmin() {
    static Admin admin("admin", "admin@ad.min", "admin1405");
    return admin;
}

void Admin::changeUserStatus(User &user, User::UserStatus status) {
    user.status = status;
}

//in the name of ALLAH
//YA MAHDI

#include "Admin.h"

Admin::Admin(std::string userName, std::string email, std::string password) :User(userName, email, password){
}

Admin & Admin::getAdmin() {
    static Admin admin("admin", "admin@ad.min", "admin1405");
    return admin;
}

//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_ADMIN_H
#define KETAB_E_MAN_ADMIN_H
#include "../User.h"


class Admin :public User{
    Admin(std::string userName, std::string email, std::string password);
    public:
    Admin & getAdmin();
};


#endif //KETAB_E_MAN_ADMIN_H

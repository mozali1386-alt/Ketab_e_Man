//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_ADMIN_H
#define KETAB_E_MAN_ADMIN_H
#include "../User.h"


class Admin :public User{
    Admin(std::string userName, std::string email, std::string password);
    uint64_t generateUserId() override;
    public:
    Admin & getAdmin();
};

زاش


#endif //KETAB_E_MAN_ADMIN_H

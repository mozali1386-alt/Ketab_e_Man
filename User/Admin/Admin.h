//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_ADMIN_H
#define KETAB_E_MAN_ADMIN_H
#include "../User.h"


class Admin :public User{

private:
    Admin(const std::string & userName, const std::string & email, const std::string & password);
    uint64_t generateUserId() override;
public:
    static Admin & getAdmin();
    void changeUserStatus(User & user, User::UserStatus status);
};

#endif //KETAB_E_MAN_ADMIN_H

//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_PUBLISHER_H
#define KETAB_E_MAN_PUBLISHER_H

#include "../User.h"

class Publisher : public User {
    std::string name;
    uint64_t generateUserId() override;
    public:
    Publisher(const std::string & name, const std::string & userName, const std::string & email, const std::string & password);

};


#endif //KETAB_E_MAN_PUBLISHER_H

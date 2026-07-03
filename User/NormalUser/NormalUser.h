//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_NORMALUSER_H
#define KETAB_E_MAN_NORMALUSER_H
#include "../User.h"

class NormalUser : public User{
private:
    std::string name;
    std::string lastName;
    double walletBalance{0};
    uint64_t favoriteGenreIds[3]{0};
    uint64_t personalLibraryId{0};
    uint64_t activeCartId{0};
    uint64_t generateUserId() override;
public:
    NormalUser(const std::string & name, const std::string & lastName,
               const std::string & username, const std::string & email, const std::string & password);
    std::string getName() const;
    std::string getLastName() const;
    double getWalletBalance() const;
    uint64_t getPersonalLibraryId() const;
    uint64_t getActiveCartId() const;
};


#endif //KETAB_E_MAN_NORMALUSER_H

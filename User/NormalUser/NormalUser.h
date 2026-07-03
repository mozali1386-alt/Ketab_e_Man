//in the name of ALLAH
//YA MAHDI

#ifndef KETAB_E_MAN_NORMALUSER_H
#define KETAB_E_MAN_NORMALUSER_H
#include "../User.h"

class NormalUser : public User {
private:
    std::vector<std::string> favoriteGenres;
    std::vector<uint64_t> purchasedBooksIds;
    std::vector<uint64_t> savedBooksIds;
    std::vector<uint64_t> purchaseHistoryIds;
    uint64_t shoppingCartId;
    uint64_t libraryId;

protected:
    uint64_t generateUserId() override;

public:
    NormalUser(const std::string &userName,
               const std::string &password,
               const std::string &email);
    void addFavoriteGenre(const std::string &genre);
    void removeFavoriteGenre(const std::string &genre);
    std::vector<std::string> getFavoriteGenres() const;
    void addPurchasedBook(uint64_t bookId);
    void saveBook(uint64_t bookId);
    void removeSavedBook(uint64_t bookId);
    std::vector<uint64_t> getPurchasedBooks() const;
    std::vector<uint64_t> getSavedBooks() const;
    void addPurchaseHistory(uint64_t purchaseId);
    std::vector<uint64_t> getPurchaseHistory() const;
    void setShoppingCartId(uint64_t id);
    uint64_t getShoppingCartId() const;
    void setLibraryId(uint64_t id);
    uint64_t getLibraryId() const;
};

#endif //KETAB_E_MAN_NORMALUSER_H

//in the name of ALLAH
//YA MAHDI

#include "NormalUser.h"

NormalUser::NormalUser(const std::string &userName,
                       const std::string &password,
                       const std::string &email)
    : User(userName, password, email) {
    userId = generateUserId();

    shoppingCartId = 0;
    libraryId = 0;
}

uint64_t NormalUser::generateUserId() {
    if (!userId) {
        uint64_t value = static_cast<uint64_t>(qHash(this->getUserName()));
        uint64_t tenDigit = value % 10000000000ULL;
        uint64_t newUserId = 12ull * 1000000 * 1000000 * 1000000 + tenDigit * 10000 * 10000 + ++usersCounter;
        return newUserId;
    }
    return this->userId;
}

void NormalUser::addFavoriteGenre(const std::string &genre) {
    if (favoriteGenres.size() >= 3)
        return;

    if (std::find(favoriteGenres.begin(),
                  favoriteGenres.end(),
                  genre) != favoriteGenres.end())
        return;

    favoriteGenres.push_back(genre);
}

void NormalUser::removeFavoriteGenre(const std::string &genre) {
    auto it = std::find(favoriteGenres.begin(),
                        favoriteGenres.end(),
                        genre);

    if (it != favoriteGenres.end())
        favoriteGenres.erase(it);
}

std::vector<std::string> NormalUser::getFavoriteGenres() const {
    return favoriteGenres;
}

void NormalUser::addPurchasedBook(uint64_t bookId) {
    purchasedBooksIds.push_back(bookId);
}

void NormalUser::saveBook(uint64_t bookId) {
    if (std::find(savedBooksIds.begin(),
                  savedBooksIds.end(),
                  bookId) == savedBooksIds.end()) {
        savedBooksIds.push_back(bookId);
    }
}

void NormalUser::removeSavedBook(uint64_t bookId) {
    auto it = std::find(savedBooksIds.begin(),
                        savedBooksIds.end(),
                        bookId);

    if (it != savedBooksIds.end())
        savedBooksIds.erase(it);
}

std::vector<uint64_t> NormalUser::getPurchasedBooks() const {
    return purchasedBooksIds;
}

std::vector<uint64_t> NormalUser::getSavedBooks() const {
    return savedBooksIds;
}

void NormalUser::addPurchaseHistory(uint64_t purchaseId) {
    purchaseHistoryIds.push_back(purchaseId);
}

std::vector<uint64_t> NormalUser::getPurchaseHistory() const {
    return purchaseHistoryIds;
}

void NormalUser::setShoppingCartId(uint64_t id) {
    shoppingCartId = id;
}

uint64_t NormalUser::getShoppingCartId() const {
    return shoppingCartId;
}

void NormalUser::setLibraryId(uint64_t id) {
    libraryId = id;
}

uint64_t NormalUser::getLibraryId() const {
    return libraryId;
}
` ` `

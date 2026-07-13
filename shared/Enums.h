//in the name of ALLAH
//YA MAHDI

#ifndef ENUMS_H
#define ENUMS_H

enum class Role {
    USER = 0,
    PUBLISHER = 1,
    ADMIN = 2
};

enum class Genre {
    FICTION = 0,
    SCIENCE = 1,
    HISTORY = 2,
    FANTASY = 3,
    ROMANCE = 4,
    MYSTERY = 5,
    TECH = 6
};

enum class NotificationType {
    NEW_BOOK_IN_FAVORITE_GENRE = 0,
    DISCOUNT_ON_SAVED_BOOK = 1,
    NEW_SALE_FOR_PUBLISHER = 2,
    PURCHASE_COMPLETE = 3
};

enum class TransactionType {
    DEPOSIT = 0,
    WITHDRAW = 1,
    PURCHASE = 2,
    SALE_INCOME = 3
};

#endif

//in the name of ALLAH
//YA MAHDI

#ifndef ENUMS_H
#define ENUMS_H

#include <QString>

enum class Role {
    USER = 0,
    PUBLISHER = 1,
    ADMIN = 2
};


enum class Genre {
    ROMANCE,
    CRIME,
    SCIFI,
    HORROR,
    CLASSIC,
    ART,
    COMEDY,
    HISTORY
};

QString genreToString(Genre genre);

bool stringToGenre(const QString &text, Genre &out);

enum class NotificationType {
    NEW_BOOK_IN_FAVORITE_GENRE = 0,
    DISCOUNT_ON_SAVED_BOOK = 1,
    NEW_SALE_FOR_PUBLISHER = 2,
    PURCHASE_COMPLETE = 3,
    NEW_REVIEW_ON_BOOK = 4
};

enum class TransactionType {
    DEPOSIT = 0,
    WITHDRAW = 1,
    PURCHASE = 2,
    SALE_INCOME = 3
};

#endif
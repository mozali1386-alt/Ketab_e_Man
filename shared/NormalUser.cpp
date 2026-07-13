//in the name of ALLAH
//YA MAHDI

#include "NormalUser.h"
#include <QStringList>

NormalUser::NormalUser() {
    role = Role::USER;
    libraryId = 0;
}

NormalUser::~NormalUser() {
}

quint64 NormalUser::getLibraryId() const {
    return libraryId;
}

QVector<Genre> NormalUser::getFavoriteGenres() const {
    return favoriteGenres;
}

void NormalUser::setLibraryId(quint64 newLibraryId) {
    libraryId = newLibraryId;
    touchUpdatedAt();
}

void NormalUser::addFavoriteGenre(Genre genre) {
    for (int i = 0; i < favoriteGenres.size(); i++) {
        if (favoriteGenres.at(i) == genre) {
            return;
        }
    }

    if (favoriteGenres.size() >= 3) {
        return;
    }

    favoriteGenres.append(genre);
    touchUpdatedAt();
}

void NormalUser::removeFavoriteGenre(Genre genre) {
    for (int i = 0; i < favoriteGenres.size(); i++) {
        if (favoriteGenres.at(i) == genre) {
            favoriteGenres.remove(i);
            break;
        }
    }
    touchUpdatedAt();
}

void NormalUser::purchaseBook(quint64 bookId) {
    Q_UNUSED(bookId);
    touchUpdatedAt();
}

quint64 NormalUser::generateId() {
    return BaseEntity::generateId();
}

QString NormalUser::serialize() const {
    QStringList genreList;
    for (int i = 0; i < favoriteGenres.size(); i++) {
        genreList.append(QString::number(static_cast<int>(favoriteGenres.at(i))));
    }

    QString result = serializeUserFields();
    result += "|" + QString::number(libraryId);
    result += "|" + genreList.join(",");

    return result;
}

void NormalUser::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int nextIndex = deserializeUserFields(tokens, 0);

    libraryId = tokens.at(nextIndex).toULongLong();
    nextIndex++;

    favoriteGenres.clear();
    QString genreToken = tokens.at(nextIndex);
    nextIndex++;
    if (genreToken.length() > 0) {
        QStringList parts = genreToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            favoriteGenres.append(static_cast<Genre>(parts.at(i).toInt()));
        }
    }
}

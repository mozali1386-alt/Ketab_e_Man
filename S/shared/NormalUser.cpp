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

QSet<Genre> NormalUser::getFavoriteGenres() const {
    return favoriteGenres;
}

void NormalUser::setLibraryId(quint64 newLibraryId) {
    libraryId = newLibraryId;
    touchUpdatedAt();
}

void NormalUser::addFavoriteGenre(Genre genre) {
    if (favoriteGenres.contains(genre)) {
        return;
    }

    if (favoriteGenres.size() >= 3) {
        return;
    }

    favoriteGenres.insert(genre);
    touchUpdatedAt();
}

void NormalUser::removeFavoriteGenre(Genre genre) {
    favoriteGenres.remove(genre);
    touchUpdatedAt();
}

quint64 NormalUser::generateId() {
    return BaseEntity::generateId();
}

QString NormalUser::serialize() const {
    QStringList genreList;
    for (Genre genre: favoriteGenres) {
        genreList.append(QString::number(static_cast<int>(genre)));
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
            favoriteGenres.insert(static_cast<Genre>(parts.at(i).toInt()));
        }
    }
}

//in the name of ALLAH
//YA MAHDI

#ifndef NORMALUSER_H
#define NORMALUSER_H

#include "User.h"
#include "Enums.h"
#include <QVector>

class NormalUser : public User {
public:
    NormalUser();

    virtual ~NormalUser();

    quint64 getLibraryId() const;

    QVector<Genre> getFavoriteGenres() const;

    void setLibraryId(quint64 newLibraryId);

    void addFavoriteGenre(Genre genre);

    void removeFavoriteGenre(Genre genre);

    void purchaseBook(quint64 bookId);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 libraryId;
    QVector<Genre> favoriteGenres;
};

#endif

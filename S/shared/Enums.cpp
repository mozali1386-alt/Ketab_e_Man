//in the name of ALLAH
//YA MAHDI

#include "Enums.h"
#include <QVector>

namespace {
    struct GenreEntry {
        Genre genre;
        QString name;
    };


    const QVector<GenreEntry> genreTable = {
        {Genre::ROMANCE, "ROMANCE"},
        {Genre::CRIME, "CRIME"},
        {Genre::SCIFI, "SCIFI"},
        {Genre::HORROR, "HORROR"},
        {Genre::CLASSIC, "CLASSIC"},
        {Genre::ART, "ART"},
        {Genre::COMEDY, "COMEDY"},
        {Genre::HISTORY, "HISTORY"},
    };
}

QString genreToString(Genre genre) {
    for (int i = 0; i < genreTable.size(); i++) {
        if (genreTable.at(i).genre == genre) {
            return genreTable.at(i).name;
        }
    }
    return QString();
}

bool stringToGenre(const QString &text, Genre &out) {
    for (int i = 0; i < genreTable.size(); i++) {
        if (genreTable.at(i).name == text) {
            out = genreTable.at(i).genre;
            return true;
        }
    }
    return false;
}

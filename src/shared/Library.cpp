//in the name of ALLAH
//YA MAHDI

#include "Library.h"
#include <QStringList>

Library::Library() {
    ownerId = 0;
}

Library::~Library() {
}

quint64 Library::getOwnerId() const {
    return ownerId;
}

QVector<quint64> Library::getShelves() const {
    return shelfIds;
}

QVector<quint64> Library::getPurchasedBooks() const {
    return purchasedBookIds;
}

QVector<quint64> Library::getSavedBooks() const {
    return savedBookIds;
}

void Library::setOwnerId(quint64 newOwnerId) {
    ownerId = newOwnerId;
    touchUpdatedAt();
}

void Library::addShelf(quint64 shelfId) {
    shelfIds.append(shelfId);
    touchUpdatedAt();
}

void Library::removeShelf(quint64 shelfId) {
    for (int i = 0; i < shelfIds.size(); i++) {
        if (shelfIds.at(i) == shelfId) {
            shelfIds.remove(i);
            break;
        }
    }
    touchUpdatedAt();
}

void Library::addToSavedBooks(quint64 bookId) {
    for (int i = 0; i < savedBookIds.size(); i++) {
        if (savedBookIds.at(i) == bookId) {
            return;
        }
    }
    savedBookIds.append(bookId);
    touchUpdatedAt();
}

void Library::removeFromSavedBooks(quint64 bookId) {
    for (int i = 0; i < savedBookIds.size(); i++) {
        if (savedBookIds.at(i) == bookId) {
            savedBookIds.remove(i);
            break;
        }
    }
    touchUpdatedAt();
}

void Library::addToPurchasedBooks(quint64 bookId) {
    purchasedBookIds.append(bookId);

    removeFromSavedBooks(bookId);

    touchUpdatedAt();
}

quint64 Library::generateId() {
    return BaseEntity::generateId();
}

QString Library::serialize() const {
    QStringList shelfList;
    for (int i = 0; i < shelfIds.size(); i++) {
        shelfList.append(QString::number(shelfIds.at(i)));
    }

    QStringList purchasedList;
    for (int i = 0; i < purchasedBookIds.size(); i++) {
        purchasedList.append(QString::number(purchasedBookIds.at(i)));
    }

    QStringList savedList;
    for (int i = 0; i < savedBookIds.size(); i++) {
        savedList.append(QString::number(savedBookIds.at(i)));
    }

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(ownerId) + "|";
    result += shelfList.join(",") + "|";
    result += purchasedList.join(",") + "|";
    result += savedList.join(",");

    return result;
}

void Library::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    ownerId = tokens.at(index).toULongLong();
    index++;

    shelfIds.clear();
    QString shelfToken = tokens.at(index);
    index++;
    if (shelfToken.length() > 0) {
        QStringList parts = shelfToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            shelfIds.append(parts.at(i).toULongLong());
        }
    }

    purchasedBookIds.clear();
    QString purchasedToken = tokens.at(index);
    index++;
    if (purchasedToken.length() > 0) {
        QStringList parts = purchasedToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            purchasedBookIds.append(parts.at(i).toULongLong());
        }
    }

    savedBookIds.clear();
    QString savedToken = tokens.at(index);
    index++;
    if (savedToken.length() > 0) {
        QStringList parts = savedToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            savedBookIds.append(parts.at(i).toULongLong());
        }
    }
}

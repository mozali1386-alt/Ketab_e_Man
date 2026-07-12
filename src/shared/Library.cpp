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

QSet<quint64> Library::getShelves() const {
    return shelfIds;
}

QSet<quint64> Library::getPurchasedBooks() const {
    return purchasedBookIds;
}

QSet<quint64> Library::getSavedBooks() const {
    return savedBookIds;
}

void Library::setOwnerId(quint64 newOwnerId) {
    ownerId = newOwnerId;
    touchUpdatedAt();
}

void Library::addShelf(quint64 shelfId) {
    shelfIds.insert(shelfId);
    touchUpdatedAt();
}

void Library::removeShelf(quint64 shelfId) {
    shelfIds.remove(shelfId);
    touchUpdatedAt();
}

void Library::addToSavedBooks(quint64 bookId) {
    savedBookIds.insert(bookId);
    touchUpdatedAt();
}

void Library::removeFromSavedBooks(quint64 bookId) {
    savedBookIds.remove(bookId);
    touchUpdatedAt();
}

void Library::addToPurchasedBooks(quint64 bookId) {
    purchasedBookIds.insert(bookId);

    removeFromSavedBooks(bookId);

    touchUpdatedAt();
}

quint64 Library::generateId() {
    return BaseEntity::generateId();
}

QString Library::serialize() const {
    QStringList shelfList;
    for (quint64 shelfId: shelfIds) {
        shelfList.append(QString::number(shelfId));
    }

    QStringList purchasedList;
    for (quint64 bookId: purchasedBookIds) {
        purchasedList.append(QString::number(bookId));
    }

    QStringList savedList;
    for (quint64 bookId: savedBookIds) {
        savedList.append(QString::number(bookId));
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
            shelfIds.insert(parts.at(i).toULongLong());
        }
    }

    purchasedBookIds.clear();
    QString purchasedToken = tokens.at(index);
    index++;
    if (purchasedToken.length() > 0) {
        QStringList parts = purchasedToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            purchasedBookIds.insert(parts.at(i).toULongLong());
        }
    }

    savedBookIds.clear();
    QString savedToken = tokens.at(index);
    index++;
    if (savedToken.length() > 0) {
        QStringList parts = savedToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            savedBookIds.insert(parts.at(i).toULongLong());
        }
    }
}

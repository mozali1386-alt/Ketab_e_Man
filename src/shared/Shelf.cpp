//in the name of ALLAH
//YA MAHDI

#include "Shelf.h"
#include <QStringList>

Shelf::Shelf() {
    name = "";
    ownerId = 0;
}

Shelf::~Shelf() {
}

QString Shelf::getName() const {
    return name;
}

quint64 Shelf::getOwnerId() const {
    return ownerId;
}

QSet<quint64> Shelf::getBookIds() const {
    return bookIds;
}

void Shelf::setOwnerId(quint64 newOwnerId) {
    ownerId = newOwnerId;
    touchUpdatedAt();
}

void Shelf::addBook(quint64 bookId) {
    bookIds.insert(bookId);
    touchUpdatedAt();
}

void Shelf::removeBook(quint64 bookId) {
    bookIds.remove(bookId);
    touchUpdatedAt();
}

void Shelf::rename(const QString &newName) {
    name = newName;
    touchUpdatedAt();
}

quint64 Shelf::generateId() {
    return BaseEntity::generateId();
}

QString Shelf::serialize() const {
    QStringList bookList;
    for (quint64 bookId: bookIds) {
        bookList.append(QString::number(bookId));
    }

    QString safeName = name;
    safeName.replace("|", "&pipe;");

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += safeName + "|";
    result += QString::number(ownerId) + "|";
    result += bookList.join(",");

    return result;
}

void Shelf::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;

    name = tokens.at(index);
    index++;
    name.replace("&pipe;", "|");

    ownerId = tokens.at(index).toULongLong();
    index++;

    bookIds.clear();
    QString bookToken = tokens.at(index);
    index++;
    if (bookToken.length() > 0) {
        QStringList parts = bookToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            bookIds.insert(parts.at(i).toULongLong());
        }
    }
}

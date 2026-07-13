//in the name of ALLAH
//YA MAHDI

#include "Author.h"
#include <QStringList>

Author::Author() {
    fullName = "";
    biography = "";
}

Author::~Author() {
}

QString Author::getFullName() const {
    return fullName;
}

QString Author::getBiography() const {
    return biography;
}

QVector<quint64> Author::getBookIds() const {
    return bookIds;
}

void Author::setFullName(const QString &newFullName) {
    fullName = newFullName;
    touchUpdatedAt();
}

void Author::setBiography(const QString &bio) {
    biography = bio;
    touchUpdatedAt();
}

void Author::addBook(quint64 bookId) {
    bookIds.append(bookId);
    touchUpdatedAt();
}

void Author::removeBook(quint64 bookId) {
    for (int i = 0; i < bookIds.size(); i++) {
        if (bookIds.at(i) == bookId) {
            bookIds.remove(i);
            break;
        }
    }
    touchUpdatedAt();
}

quint64 Author::generateId() {
    return BaseEntity::generateId();
}

QString Author::serialize() const {
    QStringList bookList;
    for (int i = 0; i < bookIds.size(); i++) {
        bookList.append(QString::number(bookIds.at(i)));
    }

    QString safeName = fullName;
    safeName.replace("|", "&pipe;");

    QString safeBio = biography;
    safeBio.replace("|", "&pipe;");

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += safeName + "|";
    result += safeBio + "|";
    result += bookList.join(",");

    return result;
}

void Author::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;

    fullName = tokens.at(index);
    index++;
    fullName.replace("&pipe;", "|");

    biography = tokens.at(index);
    index++;
    biography.replace("&pipe;", "|");

    bookIds.clear();
    QString bookToken = tokens.at(index);
    index++;
    if (bookToken.length() > 0) {
        QStringList parts = bookToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            bookIds.append(parts.at(i).toULongLong());
        }
    }
}

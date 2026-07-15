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

QSet<quint64> Author::getBookIds() const {
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
    bookIds.insert(bookId);
    touchUpdatedAt();
}

void Author::removeBook(quint64 bookId) {
    bookIds.remove(bookId);
    touchUpdatedAt();
}

quint64 Author::generateId() {
    return BaseEntity::generateId();
}

QString Author::serialize() const {
    QStringList bookList;
    for (quint64 bookId: bookIds) {
        bookList.append(QString::number(bookId));
    }

    QString safeName = fullName;
    safeName.replace("&pipe;", "&amp;pipe;");
    safeName.replace("|", "&pipe;");
    QString safeBio = biography;
    safeBio.replace("&pipe;", "&amp;pipe;");
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
    fullName.replace("&amp;pipe;", "&pipe;");
    biography = tokens.at(index);
    index++;
    biography.replace("&pipe;", "|");
    biography.replace("&amp;pipe;", "&pipe;");
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

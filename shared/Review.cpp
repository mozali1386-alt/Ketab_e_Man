//in the name of ALLAH
//YA MAHDI

#include "Review.h"
#include <QStringList>

Review::Review() {
    userId = 0;
    bookId = 0;
    text = "";
    stars = 1;
}

Review::~Review() {
}

quint64 Review::getUserId() const {
    return userId;
}

quint64 Review::getBookId() const {
    return bookId;
}

QString Review::getText() const {
    return text;
}

int Review::getStars() const {
    return stars;
}

void Review::setUserId(quint64 newUserId) {
    userId = newUserId;
    touchUpdatedAt();
}

void Review::setBookId(quint64 newBookId) {
    bookId = newBookId;
    touchUpdatedAt();
}

void Review::editText(const QString &newText) {
    text = newText;
    touchUpdatedAt();
}

void Review::editStars(int newStars) {
    if (newStars < 1) {
        newStars = 1;
    }
    if (newStars > 5) {
        newStars = 5;
    }

    stars = newStars;
    touchUpdatedAt();
}

quint64 Review::generateId() {
    return BaseEntity::generateId();
}

QString Review::serialize() const {
    QString safeText = text;
    safeText.replace("|", "&pipe;");

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(userId) + "|";
    result += QString::number(bookId) + "|";
    result += safeText + "|";
    result += QString::number(stars);

    return result;
}

void Review::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    userId = tokens.at(index).toULongLong();
    index++;
    bookId = tokens.at(index).toULongLong();
    index++;

    text = tokens.at(index);
    index++;
    text.replace("&pipe;", "|");

    stars = tokens.at(index).toInt();
    index++;
}

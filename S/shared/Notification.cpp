//in the name of ALLAH
//YA MAHDI

#include "Notification.h"
#include <QStringList>

Notification::Notification() {
    recipientId = 0;
    message = "";
    type = NotificationType::NEW_BOOK_IN_FAVORITE_GENRE;
    isRead = false;
}

Notification::~Notification() {
}

quint64 Notification::getRecipientId() const {
    return recipientId;
}

QString Notification::getMessage() const {
    return message;
}

NotificationType Notification::getType() const {
    return type;
}

bool Notification::getIsRead() const {
    return isRead;
}

void Notification::setRecipientId(quint64 newRecipientId) {
    recipientId = newRecipientId;
    touchUpdatedAt();
}

void Notification::setMessage(const QString &newMessage) {
    message = newMessage;
    touchUpdatedAt();
}

void Notification::setType(NotificationType newType) {
    type = newType;
    touchUpdatedAt();
}

void Notification::markAsRead() {
    isRead = true;
    touchUpdatedAt();
}

quint64 Notification::generateId() {
    return BaseEntity::generateId();
}

QString Notification::serialize() const {
    QString safeMessage = message;
    safeMessage.replace("&pipe;", "&amp;pipe;");
    safeMessage.replace("|", "&pipe;");
    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(recipientId) + "|";
    result += safeMessage + "|";
    result += QString::number(static_cast<int>(type)) + "|";
    result += QString(isRead ? "1" : "0");

    return result;
}

void Notification::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    recipientId = tokens.at(index).toULongLong();
    index++;

    message = tokens.at(index);
    index++;
    message.replace("&pipe;", "|");
    message.replace("&amp;pipe;", "&pipe;");
    type = static_cast<NotificationType>(tokens.at(index).toInt());
    index++;
    isRead = (tokens.at(index) == "1");
    index++;
}

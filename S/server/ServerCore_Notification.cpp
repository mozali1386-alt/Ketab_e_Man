//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Notification.h"
#include <QVector>

void ServerCore::pushNotification(quint64 recipientUserId, NotificationType type, const QString &message)
{
    Notification *notification = new Notification();
    notification->assignNewId();
    notification->setRecipientId(recipientUserId);
    notification->setType(type);
    notification->setMessage(message);
    data.getNotificationsMap().insert(notification->getId(), notification);

    ClientHandler *onlineHandler = findClientHandlerByUserId(recipientUserId);
    if (onlineHandler != nullptr) {
        onlineHandler->sendResponse(Command::NEW_NOTIFICATION_PUSH, {
            QString::number(notification->getId()),
            message,
            notification->getCreatedAt().toString("yyyy/MM/dd - HH:mm"),
            "UNREAD"
        });
    }
}

void ServerCore::pushAdminStatsUpdate()
{
    int onlineUsers = loggedInClients.size();
    int totalUsers = data.getUsersMap().size();

    QStringList statsFields = {QString::number(onlineUsers), QString::number(totalUsers)};

    for (auto it = loggedInClients.constBegin(); it != loggedInClients.constEnd(); ++it) {
        User *user = data.getUsersMap().value(it.key(), nullptr);
        if (user != nullptr && user->getRole() == Role::ADMIN) {
            it.value()->sendResponse(Command::UPDATE_ADMIN_STATS, statsFields);
        }
    }
}

void ServerCore::handleGetNotificationsRequest(ClientHandler *handler, const QStringList &fields)
{
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QStringList idsList;
    QMap<quint64, Notification*> &notifications = data.getNotificationsMap();
    for (auto it = notifications.constBegin(); it != notifications.constEnd(); ++it) {
        if (it.value()->getRecipientId() == userId) {
            idsList.append(QString::number(it.value()->getId()));
        }
    }

    handler->sendResponse(Command::NOTIFICATIONS_RESULT, {QString::number(idsList.size()), idsList.join(",")});
}

void ServerCore::handleGetNotificationInfoRequest(ClientHandler *handler, const QStringList &fields)
{
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 notifId = fields.at(0).toULongLong();
    Notification *notification = data.getNotificationsMap().value(notifId, nullptr);
    if (notification == nullptr || notification->getRecipientId() != userId) {
        handler->sendResponse(Command::FAIL, {"Notification not found"});
        return;
    }

    QString status = notification->getIsRead() ? "READ" : "UNREAD";

    handler->sendResponse(Command::NOTIFICATION_INFO_RESULT, {
        QString::number(notifId),
        notification->getMessage(),
        notification->getCreatedAt().toString("yyyy/MM/dd"),
        status
    });
}

void ServerCore::handleMarkNotificationReadRequest(ClientHandler *handler, const QStringList &fields)
{
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::MARK_READ_RESULT, {"0", "FAILED"});
        return;
    }

    quint64 notifId = fields.at(0).toULongLong();
    Notification *notification = data.getNotificationsMap().value(notifId, nullptr);
    if (notification == nullptr || notification->getRecipientId() != userId) {
        handler->sendResponse(Command::MARK_READ_RESULT, {QString::number(notifId), "FAILED"});
        return;
    }

    data.getNotificationsMap().remove(notifId);
    delete notification;

    handler->sendResponse(Command::MARK_READ_RESULT, {QString::number(notifId), "SUCCESS"});
}

void ServerCore::handleMarkAllNotificationsReadRequest(ClientHandler *handler, const QStringList &fields)
{
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    QMap<quint64, Notification*> &notifications = data.getNotificationsMap();

    QVector<quint64> idsToRemove;
    for (auto it = notifications.constBegin(); it != notifications.constEnd(); ++it) {
        if (it.value()->getRecipientId() == userId) {
            idsToRemove.append(it.key());
        }
    }

    for (quint64 notifId : idsToRemove) {
        Notification *notification = notifications.value(notifId, nullptr);
        if (notification != nullptr) {
            notifications.remove(notifId);
            delete notification;
        }
    }

    handler->sendResponse(Command::MARK_ALL_READ_RESULT, {"SUCCESS"});
}

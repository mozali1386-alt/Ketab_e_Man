//in the name of ALLAH
//YA MAHDI

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "BaseEntity.h"
#include "Enums.h"
#include <QString>

class Notification : public BaseEntity {
public:
    Notification();

    virtual ~Notification();

    quint64 getRecipientId() const;

    QString getMessage() const;

    NotificationType getType() const;

    bool getIsRead() const;

    void setRecipientId(quint64 newRecipientId);

    void setMessage(const QString &newMessage);

    void setType(NotificationType newType);

    void markAsRead();

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 recipientId;
    QString message;
    NotificationType type;
    bool isRead;
};

#endif

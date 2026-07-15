//in the name of ALLAH
//YA MAHDI

#ifndef USER_H
#define USER_H

#include "BaseEntity.h"
#include "Enums.h"
#include "Notification.h"
#include <QString>
#include <QSet>
#include <QMap>

class User : public BaseEntity {
public:
    User();

    virtual ~User();

    Role getRole() const;

    QString getFullName() const;

    QString getUsername() const;

    QString getEmail() const;

    QString getSecurityQuestion() const;

    quint64 getWalletId() const;

    QSet<quint64> getReviewIds() const;

    QSet<quint64> getNotificationIds() const;

    bool getIsBlocked() const;

    void setRole(Role newRole);

    void setFullName(const QString &newFullName);

    void setUsername(const QString &newUsername);

    void setEmail(const QString &newEmail);

    void setSecurityQuestion(const QString &question);

    void setWalletId(quint64 newWalletId);

    void setIsBlocked(bool blocked);

    bool authenticatePassword(const QString &plainPassword);

    void setPassword(const QString &plainPassword);

    void setSecurityAnswer(const QString &plainAnswer);

    bool resetPassword(const QString &question, const QString &answer, const QString &newPassword);

    void addReview(quint64 reviewId);

    void removeReview(quint64 reviewId);

    void addNotification(quint64 notifId);

    int getUnreadNotificationCount(const QMap<quint64, Notification *> &allNotifications) const;

protected:
    Role role;
    QString fullName;
    QString username;
    QString email;
    QString passwordHash;
    QString securityQuestion;
    QString securityAnswerHash;
    quint64 walletId;
    QSet<quint64> reviewIds;
    QSet<quint64> notificationIds;
    bool isBlocked;

    QString hashText(const QString &plainText) const;

    QString serializeUserFields() const;

    int deserializeUserFields(const QStringList &tokens, int startIndex);
};

#endif

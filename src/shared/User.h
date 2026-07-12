//in the name of ALLAH
//YA MAHDI

#ifndef USER_H
#define USER_H

#include "BaseEntity.h"
#include "Enums.h"
#include <QString>
#include <QVector>

class User : public BaseEntity {
public:
    User();

    virtual ~User();

    Role getRole() const;

    QString getUsername() const;

    QString getEmail() const;

    QString getSecurityQuestion() const;

    quint64 getWalletId() const;

    QVector<quint64> getReviewIds() const;

    QVector<quint64> getNotificationIds() const;

    bool getIsBlocked() const;

    bool getIsActive() const;

    void setRole(Role newRole);

    void setUsername(const QString &newUsername);

    void setEmail(const QString &newEmail);

    void setSecurityQuestion(const QString &question);

    void setWalletId(quint64 newWalletId);

    void setIsBlocked(bool blocked);

    void setIsActive(bool active);

    bool authenticatePassword(const QString &plainPassword);

    void setPassword(const QString &plainPassword);

    void setSecurityAnswer(const QString &plainAnswer);

    bool resetPassword(const QString &question, const QString &answer, const QString &newPassword);

    void addReview(quint64 reviewId);

    void removeReview(quint64 reviewId);

    void addNotification(quint64 notifId);

    int getUnreadNotificationCount() const;

protected:
    Role role;
    QString username;
    QString email;
    QString passwordHash;
    QString securityQuestion;
    QString securityAnswerHash;
    quint64 walletId;
    QVector<quint64> reviewIds;
    QVector<quint64> notificationIds;
    bool isBlocked;
    bool isActive;

    QString hashText(const QString &plainText) const;

    QString serializeUserFields() const;

    int deserializeUserFields(const QStringList &tokens, int startIndex);
};

#endif

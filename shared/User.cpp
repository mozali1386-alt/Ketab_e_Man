//in the name of ALLAH
//YA MAHDI

#include "User.h"
#include <QCryptographicHash>
#include <QStringList>

User::User() {
    role = Role::USER;
    fullName = "";
    username = "";
    email = "";
    passwordHash = "";
    securityQuestion = "";
    securityAnswerHash = "";
    walletId = 0;
    isBlocked = false;
}

User::~User() {
}

Role User::getRole() const {
    return role;
}

QString User::getFullName() const {
    return fullName;
}

QString User::getUsername() const {
    return username;
}

QString User::getEmail() const {
    return email;
}

QString User::getSecurityQuestion() const {
    return securityQuestion;
}

quint64 User::getWalletId() const {
    return walletId;
}

QSet<quint64> User::getReviewIds() const {
    return reviewIds;
}

QSet<quint64> User::getNotificationIds() const {
    return notificationIds;
}

bool User::getIsBlocked() const {
    return isBlocked;
}

void User::setRole(Role newRole) {
    role = newRole;
    touchUpdatedAt();
}

void User::setFullName(const QString &newFullName) {
    fullName = newFullName;
    touchUpdatedAt();
}

void User::setUsername(const QString &newUsername) {
    username = newUsername;
    touchUpdatedAt();
}

void User::setEmail(const QString &newEmail) {
    email = newEmail;
    touchUpdatedAt();
}

void User::setSecurityQuestion(const QString &question) {
    securityQuestion = question;
    touchUpdatedAt();
}

void User::setWalletId(quint64 newWalletId) {
    walletId = newWalletId;
    touchUpdatedAt();
}

void User::setIsBlocked(bool blocked) {
    isBlocked = blocked;
    touchUpdatedAt();
}

QString User::hashText(const QString &plainText) const {
    QByteArray bytes = plainText.toUtf8();
    QByteArray hashed = QCryptographicHash::hash(bytes, QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

bool User::authenticatePassword(const QString &plainPassword) {
    QString candidateHash = hashText(plainPassword);
    if (candidateHash == passwordHash) {
        return true;
    }
    return false;
}

void User::setPassword(const QString &plainPassword) {
    passwordHash = hashText(plainPassword);
    touchUpdatedAt();
}

void User::setSecurityAnswer(const QString &plainAnswer) {
    securityAnswerHash = hashText(plainAnswer);
    touchUpdatedAt();
}

bool User::resetPassword(const QString &question, const QString &answer, const QString &newPassword) {
    if (question != securityQuestion) {
        return false;
    }

    QString candidateAnswerHash = hashText(answer);
    if (candidateAnswerHash != securityAnswerHash) {
        return false;
    }

    setPassword(newPassword);
    return true;
}

void User::addReview(quint64 reviewId) {
    reviewIds.insert(reviewId);
    touchUpdatedAt();
}

void User::removeReview(quint64 reviewId) {
    reviewIds.remove(reviewId);
    touchUpdatedAt();
}

void User::addNotification(quint64 notifId) {
    notificationIds.insert(notifId);
    touchUpdatedAt();
}

int User::getUnreadNotificationCount(const QMap<quint64, Notification *> &allNotifications) const {
    int unreadCount = 0;
    for (quint64 notifId: notificationIds) {
        Notification *notif = allNotifications.value(notifId, nullptr);
        if (notif != nullptr && !notif->getIsRead()) {
            unreadCount++;
        }
    }
    return unreadCount;
}

QString User::serializeUserFields() const {
    QStringList reviewList;
    for (quint64 reviewId: reviewIds) {
        reviewList.append(QString::number(reviewId));
    }

    QStringList notifList;
    for (quint64 notifId: notificationIds) {
        notifList.append(QString::number(notifId));
    }

    QString safeFullName = fullName;
    safeFullName.replace("&pipe;", "&amp;pipe;");
    safeFullName.replace("|", "&pipe;");
    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += QString::number(static_cast<int>(role)) + "|";
    result += safeFullName + "|";
    result += username + "|";
    result += email + "|";
    result += passwordHash + "|";
    result += securityQuestion + "|";
    result += securityAnswerHash + "|";
    result += QString::number(walletId) + "|";
    result += QString(isBlocked ? "1" : "0") + "|";
    result += reviewList.join(",") + "|";
    result += notifList.join(",");

    return result;
}

int User::deserializeUserFields(const QStringList &tokens, int startIndex) {
    int index = startIndex;

    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    role = static_cast<Role>(tokens.at(index).toInt());
    index++;

    fullName = tokens.at(index);
    index++;
    fullName.replace("&pipe;", "|");
    fullName.replace("&amp;pipe;", "&pipe;");
    username = tokens.at(index);
    index++;
    email = tokens.at(index);
    index++;
    passwordHash = tokens.at(index);
    index++;
    securityQuestion = tokens.at(index);
    index++;
    securityAnswerHash = tokens.at(index);
    index++;
    walletId = tokens.at(index).toULongLong();
    index++;
    isBlocked = (tokens.at(index) == "1");
    index++;

    reviewIds.clear();
    QString reviewsToken = tokens.at(index);
    index++;
    if (reviewsToken.length() > 0) {
        QStringList parts = reviewsToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            reviewIds.insert(parts.at(i).toULongLong());
        }
    }

    notificationIds.clear();
    QString notifToken = tokens.at(index);
    index++;
    if (notifToken.length() > 0) {
        QStringList parts = notifToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            notificationIds.insert(parts.at(i).toULongLong());
        }
    }

    return index;
}

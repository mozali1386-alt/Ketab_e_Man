//in the name of ALLAH
//YA MAHDI

#include "Admin.h"
#include <QStringList>

Admin::Admin() {
    role = Role::ADMIN;
}

Admin::~Admin() {
}

void Admin::blockUser(User *target) {
    if (target != nullptr) {
        target->setIsBlocked(true);
    }
    touchUpdatedAt();
}

void Admin::unblockUser(User *target) {
    if (target != nullptr) {
        target->setIsBlocked(false);
    }
    touchUpdatedAt();
}

quint64 Admin::generateId() {
    return BaseEntity::generateId();
}

QString Admin::serialize() const {
    return serializeUserFields();
}

void Admin::deserialize(const QString &data) {
    QStringList tokens = data.split("|");
    deserializeUserFields(tokens, 0);
}

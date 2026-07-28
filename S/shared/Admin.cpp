//in the name of ALLAH
//YA MAHDI

#include "Admin.h"
#include <QStringList>

Admin::Admin() {
    role = Role::ADMIN;
}

Admin::~Admin() {
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

//in the name of ALLAH
//YA MAHDI

#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User {
public:
    Admin();

    virtual ~Admin();

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;
};

#endif

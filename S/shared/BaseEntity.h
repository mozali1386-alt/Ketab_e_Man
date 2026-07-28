//in the name of ALLAH
//YA MAHDI

#ifndef BASEENTITY_H
#define BASEENTITY_H

#include <QString>
#include <QDateTime>

class BaseEntity {
public:
    BaseEntity();

    virtual ~BaseEntity();

    quint64 getId() const;

    QDateTime getCreatedAt() const;

    QDateTime getUpdatedAt() const;

    void assignNewId();

    virtual QString serialize() const = 0;

    virtual void deserialize(const QString &data) = 0;

    static quint64 generateId();

    static void registerExistingId(quint64 existingId);

protected:
    quint64 id;
    QDateTime createdAt;
    QDateTime updatedAt;

    void setId(quint64 newId);

    void touchUpdatedAt();

private:
    static quint64 idCounter;
};

#endif

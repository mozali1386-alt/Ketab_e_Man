//in the name of ALLAH
//YA MAHDI

#ifndef SHELF_H
#define SHELF_H

#include "BaseEntity.h"
#include <QString>
#include <QVector>

class Shelf : public BaseEntity {
public:
    Shelf();

    virtual ~Shelf();

    QString getName() const;

    quint64 getOwnerId() const;

    QVector<quint64> getBookIds() const;

    void setOwnerId(quint64 newOwnerId);

    void addBook(quint64 bookId);

    void removeBook(quint64 bookId);

    void rename(const QString &newName);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    QString name;
    quint64 ownerId;
    QVector<quint64> bookIds;
};

#endif

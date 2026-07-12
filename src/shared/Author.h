//in the name of ALLAH
//YA MAHDI

#ifndef AUTHOR_H
#define AUTHOR_H

#include "BaseEntity.h"
#include <QString>
#include <QVector>

class Author : public BaseEntity {
public:
    Author();

    virtual ~Author();

    QString getFullName() const;

    QString getBiography() const;

    QVector<quint64> getBookIds() const;

    void setFullName(const QString &newFullName);

    void setBiography(const QString &bio);

    void addBook(quint64 bookId);

    void removeBook(quint64 bookId);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    QString fullName;
    QString biography;
    QVector<quint64> bookIds;
};

#endif

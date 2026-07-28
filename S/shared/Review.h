//in the name of ALLAH
//YA MAHDI

#ifndef REVIEW_H
#define REVIEW_H

#include "BaseEntity.h"
#include <QString>

class Review : public BaseEntity {
public:
    Review();

    virtual ~Review();

    quint64 getUserId() const;

    quint64 getBookId() const;

    QString getText() const;

    int getStars() const;

    void setUserId(quint64 newUserId);

    void setBookId(quint64 newBookId);

    void editText(const QString &newText);

    void editStars(int newStars);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    quint64 userId;
    quint64 bookId;
    QString text;
    int stars;
};

#endif

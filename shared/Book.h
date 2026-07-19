//in the name of ALLAH
//YA MAHDI

#ifndef BOOK_H
#define BOOK_H

#include "BaseEntity.h"
#include "Enums.h"
#include <QString>
#include <QSet>

class Book : public BaseEntity {
public:
    Book();

    virtual ~Book();

    QString getTitle() const;

    quint64 getAuthorId() const;

    quint64 getPublisherId() const;

    Genre getGenre() const;

    QString getDescription() const;

    double getPrice() const;

    double getDiscountPercent() const;

    QString getCoverImagePath() const;

    QString getPdfFilePath() const;

    int getSalesCount() const;

    bool getIsActive() const;

    QSet<quint64> getReviewIds() const;

    void setTitle(const QString &newTitle);

    void setAuthorId(quint64 newAuthorId);

    void setPublisherId(quint64 newPublisherId);

    void setGenre(Genre newGenre);

    void setDescription(const QString &newDescription);

    void setPrice(double newPrice);

    void setCoverImagePath(const QString &path);

    void setPdfFilePath(const QString &path);

    void applyDiscount(double percent);

    double getFinalPrice() const;

    void incrementSales();

    void deactivate();

    void reactivate();

    void addReview(quint64 reviewId);

    void removeReview(quint64 reviewId);

    static quint64 generateId();

    QString serialize() const override;

    void deserialize(const QString &data) override;

private:
    QString title;
    quint64 authorId;
    quint64 publisherId;
    Genre genre;
    QString description;
    double price;
    double discountPercent;
    QString coverImagePath;
    QString pdfFilePath;
    int salesCount;
    bool isActive;
    QSet<quint64> reviewIds;
};

#endif

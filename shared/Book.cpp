//in the name of ALLAH
//YA MAHDI

#include "Book.h"
#include <QStringList>

Book::Book() {
    title = "";
    authorId = 0;
    publisherId = 0;
    genre = Genre::FICTION;
    description = "";
    price = 0.0;
    discountPercent = 0.0;
    coverImagePath = "";
    pdfFilePath = "";
    salesCount = 0;
    isActive = true;
}

Book::~Book() {
}

QString Book::getTitle() const {
    return title;
}

quint64 Book::getAuthorId() const {
    return authorId;
}

quint64 Book::getPublisherId() const {
    return publisherId;
}

Genre Book::getGenre() const {
    return genre;
}

QString Book::getDescription() const {
    return description;
}

double Book::getPrice() const {
    return price;
}

double Book::getDiscountPercent() const {
    return discountPercent;
}

QString Book::getCoverImagePath() const {
    return coverImagePath;
}

QString Book::getPdfFilePath() const {
    return pdfFilePath;
}

int Book::getSalesCount() const {
    return salesCount;
}

bool Book::getIsActive() const {
    return isActive;
}

QVector<quint64> Book::getReviewIds() const {
    return reviewIds;
}

void Book::setTitle(const QString &newTitle) {
    title = newTitle;
    touchUpdatedAt();
}

void Book::setAuthorId(quint64 newAuthorId) {
    authorId = newAuthorId;
    touchUpdatedAt();
}

void Book::setPublisherId(quint64 newPublisherId) {
    publisherId = newPublisherId;
    touchUpdatedAt();
}

void Book::setGenre(Genre newGenre) {
    genre = newGenre;
    touchUpdatedAt();
}

void Book::setDescription(const QString &newDescription) {
    description = newDescription;
    touchUpdatedAt();
}

void Book::setPrice(double newPrice) {
    price = newPrice;
    touchUpdatedAt();
}

void Book::setCoverImagePath(const QString &path) {
    coverImagePath = path;
    touchUpdatedAt();
}

void Book::setPdfFilePath(const QString &path) {
    pdfFilePath = path;
    touchUpdatedAt();
}

void Book::applyDiscount(double percent) {
    if (percent < 0.0) {
        percent = 0.0;
    }
    if (percent > 100.0) {
        percent = 100.0;
    }

    discountPercent = percent;
    touchUpdatedAt();
}

double Book::getFinalPrice() const {
    double discountAmount = (price * discountPercent) / 100.0;
    double finalPrice = price - discountAmount;

    if (finalPrice < 0.0) {
        finalPrice = 0.0;
    }

    return finalPrice;
}

void Book::incrementSales() {
    salesCount = salesCount + 1;
    touchUpdatedAt();
}

void Book::deactivate() {
    isActive = false;
    touchUpdatedAt();
}

void Book::reactivate() {
    isActive = true;
    touchUpdatedAt();
}

void Book::addReview(quint64 reviewId) {
    reviewIds.insert(reviewId);
    touchUpdatedAt();
}

void Book::removeReview(quint64 reviewId) {
    reviewIds.remove(reviewId);
    touchUpdatedAt();
}

quint64 Book::generateId() {
    return BaseEntity::generateId();
}

QString Book::serialize() const {
    QStringList reviewList;
    for (quint64 reviewId: reviewIds) {
        reviewList.append(QString::number(reviewId));
    }

    QString safeTitle = title;
    safeTitle.replace("|", "&pipe;");

    QString safeDescription = description;
    safeDescription.replace("|", "&pipe;");

    QString safeCoverPath = coverImagePath;
    safeCoverPath.replace("|", "&pipe;");

    QString safePdfPath = pdfFilePath;
    safePdfPath.replace("|", "&pipe;");

    QString result = "";
    result += QString::number(id) + "|";
    result += createdAt.toString(Qt::ISODate) + "|";
    result += updatedAt.toString(Qt::ISODate) + "|";
    result += safeTitle + "|";
    result += QString::number(authorId) + "|";
    result += QString::number(publisherId) + "|";
    result += QString::number(static_cast<int>(genre)) + "|";
    result += safeDescription + "|";
    result += QString::number(price) + "|";
    result += QString::number(discountPercent) + "|";
    result += safeCoverPath + "|";
    result += safePdfPath + "|";
    result += QString::number(salesCount) + "|";
    result += QString(isActive ? "1" : "0") + "|";
    result += reviewList.join(",");

    return result;
}

void Book::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int index = 0;
    id = tokens.at(index).toULongLong();
    index++;
    createdAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;
    updatedAt = QDateTime::fromString(tokens.at(index), Qt::ISODate);
    index++;

    title = tokens.at(index);
    index++;
    title.replace("&pipe;", "|");

    authorId = tokens.at(index).toULongLong();
    index++;
    publisherId = tokens.at(index).toULongLong();
    index++;
    genre = static_cast<Genre>(tokens.at(index).toInt());
    index++;

    description = tokens.at(index);
    index++;
    description.replace("&pipe;", "|");

    price = tokens.at(index).toDouble();
    index++;
    discountPercent = tokens.at(index).toDouble();
    index++;

    coverImagePath = tokens.at(index);
    index++;
    coverImagePath.replace("&pipe;", "|");

    pdfFilePath = tokens.at(index);
    index++;
    pdfFilePath.replace("&pipe;", "|");

    salesCount = tokens.at(index).toInt();
    index++;
    isActive = (tokens.at(index) == "1");
    index++;

    reviewIds.clear();
    QString reviewToken = tokens.at(index);
    index++;
    if (reviewToken.length() > 0) {
        QStringList parts = reviewToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            reviewIds.insert(parts.at(i).toULongLong());
        }
    }
}

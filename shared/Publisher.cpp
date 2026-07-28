//in the name of ALLAH
//YA MAHDI

#include "Publisher.h"
#include "PipeEscape.h"
#include <QStringList>

Publisher::Publisher() {
    role = Role::PUBLISHER;
    revenue = 0;
    bio = "";
}

Publisher::~Publisher() {
}

QSet<quint64> Publisher::getMyBookIds() const {
    return myBookIds;
}

qint64 Publisher::getRevenue() const {
    return revenue;
}

QString Publisher::getBio() const {
    return bio;
}

void Publisher::publishBook(quint64 bookId) {
    myBookIds.insert(bookId);
    touchUpdatedAt();
}

void Publisher::removeBook(quint64 bookId) {
    myBookIds.remove(bookId);
    touchUpdatedAt();
}

void Publisher::receiveSaleIncome(qint64 amount) {
    revenue = revenue + amount;
    touchUpdatedAt();
}

void Publisher::setBio(const QString &newBio) {
    bio = newBio;
    touchUpdatedAt();
}

quint64 Publisher::generateId() {
    return BaseEntity::generateId();
}

QString Publisher::serialize() const {
    QStringList bookList;
    for (quint64 bookId: myBookIds) {
        bookList.append(QString::number(bookId));
    }

    QString safeBio = PipeEscape::escape(bio);

    QString result = serializeUserFields();
    result += "|" + bookList.join(",");
    result += "|" + QString::number(revenue);
    result += "|" + safeBio;

    return result;
}

void Publisher::deserialize(const QString &data) {
    QStringList tokens = data.split("|");

    int nextIndex = deserializeUserFields(tokens, 0);

    myBookIds.clear();
    QString bookToken = tokens.at(nextIndex);
    nextIndex++;
    if (bookToken.length() > 0) {
        QStringList parts = bookToken.split(",");
        for (int i = 0; i < parts.size(); i++) {
            myBookIds.insert(parts.at(i).toULongLong());
        }
    }

    revenue = tokens.at(nextIndex).toLongLong();
    nextIndex++;


    if (nextIndex < tokens.size()) {
        bio = tokens.at(nextIndex);
        nextIndex++;
        bio = PipeEscape::unescape(bio);
    } else {
        bio = "";
    }
}

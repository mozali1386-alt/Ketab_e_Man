//in the name of ALLAH
//YA MAHDI

#include "Publisher.h"
#include <QStringList>

Publisher::Publisher() {
    role = Role::PUBLISHER;
    revenue = 0.0;
}

Publisher::~Publisher() {
}

QVector<quint64> Publisher::getMyBookIds() const {
    return myBookIds;
}

double Publisher::getRevenue() const {
    return revenue;
}

void Publisher::publishBook(quint64 bookId) {
    myBookIds.append(bookId);
    touchUpdatedAt();
}

void Publisher::removeBook(quint64 bookId) {
    for (int i = 0; i < myBookIds.size(); i++) {
        if (myBookIds.at(i) == bookId) {
            myBookIds.remove(i);
            break;
        }
    }
    touchUpdatedAt();
}

void Publisher::receiveSaleIncome(double amount) {
    revenue = revenue + amount;
    touchUpdatedAt();
}

quint64 Publisher::generateId() {
    return BaseEntity::generateId();
}

QString Publisher::serialize() const {
    QStringList bookList;
    for (int i = 0; i < myBookIds.size(); i++) {
        bookList.append(QString::number(myBookIds.at(i)));
    }

    QString result = serializeUserFields();
    result += "|" + bookList.join(",");
    result += "|" + QString::number(revenue);

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
            myBookIds.append(parts.at(i).toULongLong());
        }
    }

    revenue = tokens.at(nextIndex).toDouble();
    nextIndex++;
}

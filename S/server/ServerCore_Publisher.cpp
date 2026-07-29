//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Publisher.h"
#include "../shared/Wallet.h"
#include "../shared/Transaction.h"
#include <algorithm>

QVector<quint64> ServerCore::sortedPublisherBookIdsBySales(Publisher *publisher, bool descending) {
    QSet<quint64> bookIds = publisher->getMyBookIds();
    QVector<quint64> sorted;
    for (quint64 id: bookIds) {
        Book *book = data.getBooksMap().value(id, nullptr);
        if (book == nullptr || book->getIsDeletedByAdmin()) {
            continue;
        }
        sorted.append(id);
    }

    std::sort(sorted.begin(), sorted.end(), [&](quint64 a, quint64 b) {
        int salesA = 0;
        int salesB = 0;
        Book *bookA = data.getBooksMap().value(a, nullptr);
        Book *bookB = data.getBooksMap().value(b, nullptr);
        if (bookA != nullptr) {
            salesA = bookA->getSalesCount();
        }
        if (bookB != nullptr) {
            salesB = bookB->getSalesCount();
        }
        if (descending) {
            return salesA > salesB;
        }
        return salesA < salesB;
    });

    return sorted;
}

void ServerCore::handleGetPubGeneralStatsRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Publisher *publisher = static_cast<Publisher *>(user);
    QSet<quint64> bookIds = publisher->getMyBookIds();
    int totalBooks = 0;
    double scoreSum = 0.0;
    int scoredBooks = 0;
    for (quint64 bookId: bookIds) {
        Book *book = data.getBooksMap().value(bookId, nullptr);
        if (book == nullptr || book->getIsDeletedByAdmin()) {
            continue;
        }
        totalBooks++;
        double avg = averageStarsForBook(bookId);
        if (avg > 0.0) {
            scoreSum += avg;
            scoredBooks++;
        }
    }
    double totalAvgScore = (scoredBooks > 0) ? (scoreSum / scoredBooks) : 0.0;

    qint64 totalIncome = publisher->getRevenue();

    handler->sendResponse(Command::PUB_GENERAL_RESULT, {
                              QString::number(totalBooks),
                              QString::number(totalAvgScore, 'f', 1),
                              QString::number(totalIncome)
                          });
}

void ServerCore::handleGetPubTopBooksIdsRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Publisher *publisher = static_cast<Publisher *>(user);
    QVector<quint64> sortedIds = sortedPublisherBookIdsBySales(publisher, true);

    QStringList idsList;
    for (int i = 0; i < sortedIds.size() && i < 5; i++) {
        idsList.append(QString::number(sortedIds.at(i)));
    }

    handler->sendResponse(Command::PUB_TOP_IDS_RESULT, {idsList.join(",")});
}

void ServerCore::handleGetPubBookInfoTopRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPublisherId() != user->getId() || book->getIsDeletedByAdmin()) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    handler->sendResponse(Command::PUB_BOOK_INFO_TOP_RESULT, {
                              QString::number(bookId),
                              book->getTitle(),
                              QString::number(book->getSalesCount()),
                              QString::number(averageStarsForBook(bookId), 'f', 1)
                          });
}

void ServerCore::handleGetPubLowestBooksIdsRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Publisher *publisher = static_cast<Publisher *>(user);

    QVector<quint64> topIds = sortedPublisherBookIdsBySales(publisher, true);
    QSet<quint64> topSet;
    for (int i = 0; i < topIds.size() && i < 5; i++) {
        topSet.insert(topIds.at(i));
    }

    QVector<quint64> ascendingIds = sortedPublisherBookIdsBySales(publisher, false);

    QStringList idsList;
    for (quint64 id: ascendingIds) {
        if (topSet.contains(id)) {
            continue;
        }
        idsList.append(QString::number(id));
        if (idsList.size() >= 5) {
            break;
        }
    }

    handler->sendResponse(Command::PUB_LOWEST_IDS_RESULT, {idsList.join(",")});
}

void ServerCore::handleGetPubBookInfoLowestRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr || book->getPublisherId() != user->getId() || book->getIsDeletedByAdmin()) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    handler->sendResponse(Command::PUB_BOOK_INFO_LOWEST_RESULT, {
                              QString::number(bookId),
                              book->getTitle(),
                              QString::number(book->getSalesCount()),
                              QString::number(averageStarsForBook(bookId), 'f', 1)
                          });
}

void ServerCore::handleGetPubSalesChartRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Publisher *publisher = static_cast<Publisher *>(user);
    QVector<quint64> sortedIds = sortedPublisherBookIdsBySales(publisher, true);

    int totalSales = 0;
    for (quint64 id: sortedIds) {
        Book *book = data.getBooksMap().value(id, nullptr);
        if (book != nullptr) {
            totalSales += book->getSalesCount();
        }
    }

    QStringList entries;

    if (sortedIds.size() <= 5 || totalSales == 0) {
        for (quint64 id: sortedIds) {
            Book *book = data.getBooksMap().value(id, nullptr);
            if (book != nullptr) {
                entries.append(book->getTitle() + ":" + QString::number(book->getSalesCount()));
            }
        }
    } else {
        int threshold = (totalSales * 7 + 9) / 10;
        int cumulative = 0;
        int index = 0;
        while (index < sortedIds.size() && cumulative < threshold) {
            Book *book = data.getBooksMap().value(sortedIds.at(index), nullptr);
            if (book != nullptr) {
                entries.append(book->getTitle() + ":" + QString::number(book->getSalesCount()));
                cumulative += book->getSalesCount();
            }
            index++;
        }

        int remainingSales = totalSales - cumulative;
        if (index < sortedIds.size() && remainingSales > 0) {
            entries.append(QString::fromUtf8("سایر کتاب‌ها") + ":" + QString::number(remainingSales));
        }
    }

    handler->sendResponse(Command::PUB_SALES_CHART_RESULT, {entries.join(",")});
}

void ServerCore::handleGetPubScoreChartRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Publisher *publisher = static_cast<Publisher *>(user);
    QSet<quint64> bookIds = publisher->getMyBookIds();

    QStringList entries;
    for (quint64 id: bookIds) {
        Book *book = data.getBooksMap().value(id, nullptr);
        if (book == nullptr || book->getIsDeletedByAdmin()) {
            continue;
        }
        entries.append(book->getTitle() + ":" + QString::number(averageStarsForBook(id), 'f', 1));
    }

    handler->sendResponse(Command::PUB_SCORE_CHART_RESULT, {entries.join(",")});
}

void ServerCore::handleGetPubProfileInfoRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Publisher *publisher = static_cast<Publisher *>(user);

    qint64 balance = 0;
    Wallet *wallet = data.getWalletsMap().value(user->getWalletId(), nullptr);
    if (wallet != nullptr) {
        balance = wallet->getBalance();
    }

    handler->sendResponse(Command::PUB_PROFILE_INFO, {
                              user->getFullName(),
                              user->getUsername(),
                              user->getEmail(),
                              QString::number(balance),
                              publisher->getBio()
                          });
}

void ServerCore::handleUpdatePubProfileRequest(ClientHandler *handler, const QStringList &fields) {
    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    if (fields.size() < 3) {
        handler->sendResponse(Command::UPDATE_PUB_PROFILE_RESULT, {"DUPLICATE_EMAIL"});
        return;
    }

    QString newPublisherName = fields.at(0);
    QString newEmail = fields.at(1);
    QString newBio = fields.at(2);

    if (newEmail != user->getEmail()) {
        User *existing = data.findUserByEmail(newEmail);
        if (existing != nullptr && existing->getId() != user->getId()) {
            handler->sendResponse(Command::UPDATE_PUB_PROFILE_RESULT, {"DUPLICATE_EMAIL"});
            return;
        }
    }

    if (newPublisherName != user->getFullName()) {
        QMap<quint64, User *> &users = data.getUsersMap();
        for (auto it = users.constBegin(); it != users.constEnd(); ++it) {
            User *other = it.value();
            if (other->getId() == user->getId()) {
                continue;
            }
            if (other->getRole() == Role::PUBLISHER && other->getFullName() == newPublisherName) {
                handler->sendResponse(Command::UPDATE_PUB_PROFILE_RESULT, {"DUPLICATE_ENTESHARAT"});
                return;
            }
        }
    }

    if (newEmail != user->getEmail()) {
        data.unregisterEmail(user->getEmail());
        data.registerEmail(newEmail, user->getId());
    }

    user->setFullName(newPublisherName);
    user->setEmail(newEmail);

    Publisher *publisher = static_cast<Publisher *>(user);
    publisher->setBio(newBio);

    handler->sendResponse(Command::UPDATE_PUB_PROFILE_RESULT, {"SUCCESS"});
}

void ServerCore::handleWithdrawBalanceRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    User *user = requireRole(handler, Role::PUBLISHER);
    if (user == nullptr) {
        return;
    }

    Wallet *wallet = data.getWalletsMap().value(user->getWalletId(), nullptr);
    if (wallet == nullptr) {
        handler->sendResponse(Command::WITHDRAW_BALANCE_RESULT, {"FAIL"});
        return;
    }

    qint64 balance = wallet->getBalance();
    if (balance <= 0) {
        handler->sendResponse(Command::WITHDRAW_BALANCE_RESULT, {"FAIL"});
        return;
    }

    bool withdrawOk = wallet->withdraw(balance);
    if (!withdrawOk) {
        handler->sendResponse(Command::WITHDRAW_BALANCE_RESULT, {"FAIL"});
        return;
    }

    Transaction *transaction = new Transaction();
    transaction->assignNewId();
    transaction->setWalletId(wallet->getId());
    transaction->setAmount(balance);
    transaction->setType(TransactionType::WITHDRAW);
    data.getTransactionsMap().insert(transaction->getId(), transaction);
    wallet->addTransaction(transaction->getId());

    handler->sendResponse(Command::WITHDRAW_BALANCE_RESULT, {"SUCCESS"});
}

//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Protocol.h"
#include "../shared/Publisher.h"
#include <QStringList>

bool ServerCore::processPurchase(ClientHandler *handler, quint64 userId, const QVector<quint64> &bookIdList) {
    Library *library = requireUserLibrary(userId, handler);
    if (library == nullptr) {
        return false;
    }

    User *buyerUser = data.getUsersMap().value(userId);
    Wallet *buyerWallet = data.getWalletsMap().value(buyerUser->getWalletId(), nullptr);
    if (buyerWallet == nullptr) {
        handler->sendResponse(RES_FAIL, "Wallet not found");
        return false;
    }

    if (bookIdList.size() == 0) {
        handler->sendResponse(RES_FAIL, "No books to purchase");
        return false;
    }

    double total = 0.0;
    for (int i = 0; i < bookIdList.size(); i++) {
        Book *b = data.getBooksMap().value(bookIdList.at(i), nullptr);
        if (b == nullptr || !b->getIsActive()) {
            handler->sendResponse(RES_FAIL, "One of the books is unavailable");
            return false;
        }

        User *publisherUser = data.getUsersMap().value(b->getPublisherId(), nullptr);
        if (publisherUser == nullptr || publisherUser->getRole() != Role::PUBLISHER) {
            handler->sendResponse(RES_FAIL, "One of the books has no valid publisher");
            return false;
        }

        Publisher *publisher = static_cast<Publisher *>(publisherUser);
        if (!data.getWalletsMap().contains(publisher->getWalletId())) {
            handler->sendResponse(RES_FAIL, "One of the publishers has no valid wallet");
            return false;
        }

        total += b->getFinalPrice();
    }

    if (!buyerWallet->withdraw(total)) {
        handler->sendResponse(RES_FAIL, "Insufficient balance");
        return false;
    }

    Transaction *buyerTx = new Transaction();
    buyerTx->assignNewId();
    buyerTx->setWalletId(buyerWallet->getId());
    buyerTx->setAmount(total);
    buyerTx->setType(TransactionType::PURCHASE);
    data.getTransactionsMap().insert(buyerTx->getId(), buyerTx);
    buyerWallet->addTransaction(buyerTx->getId());

    for (int i = 0; i < bookIdList.size(); i++) {
        quint64 bookId = bookIdList.at(i);
        Book *b = data.getBooksMap().value(bookId);
        b->incrementSales();
        library->addToPurchasedBooks(bookId);

        Publisher *publisher = static_cast<Publisher *>(data.getUsersMap().value(b->getPublisherId()));
        publisher->receiveSaleIncome(b->getFinalPrice());

        Wallet *publisherWallet = data.getWalletsMap().value(publisher->getWalletId());
        publisherWallet->deposit(b->getFinalPrice());

        Transaction *publisherTx = new Transaction();
        publisherTx->assignNewId();
        publisherTx->setWalletId(publisherWallet->getId());
        publisherTx->setAmount(b->getFinalPrice());
        publisherTx->setType(TransactionType::SALE_INCOME);
        data.getTransactionsMap().insert(publisherTx->getId(), publisherTx);
        publisherWallet->addTransaction(publisherTx->getId());

        QString message = "Your book '" + b->getTitle() + "' was purchased!";
        sendNotificationToUser(publisher->getId(), NotificationType::NEW_SALE_FOR_PUBLISHER, message);
    }

    Purchase *purchase = new Purchase();
    purchase->assignNewId();
    purchase->setBuyerId(userId);

    QSet<quint64> purchasedBookIdSet;
    for (int i = 0; i < bookIdList.size(); i++) {
        purchasedBookIdSet.insert(bookIdList.at(i));
    }
    purchase->setBookIds(purchasedBookIdSet);
    purchase->setTotalAmount(total);
    data.getPurchasesMap().insert(purchase->getId(), purchase);

    sendNotificationToUser(userId, NotificationType::PURCHASE_COMPLETE, "Your purchase was completed!");

    handler->sendResponse(RES_SUCCESS, QString::number(purchase->getId()) + "|" + QString::number(total));
    return true;
}


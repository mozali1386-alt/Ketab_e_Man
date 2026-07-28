//in the name of ALLAH
//YA MAHDI

#include "ServerCore.h"
#include "../shared/Publisher.h"
#include "../shared/Cart.h"

PurchaseOutcome ServerCore::performPurchase(quint64 buyerId, const QSet<quint64> &bookIds) {
    PurchaseOutcome outcome;
    outcome.success = false;
    outcome.purchaseId = 0;
    outcome.totalAmount = 0;

    User *buyer = data.getUsersMap().value(buyerId, nullptr);
    if (buyer == nullptr || buyer->getRole() != Role::USER) {
        outcome.failReason = "Not authorized";
        return outcome;
    }

    NormalUser *normalUser = static_cast<NormalUser *>(buyer);
    Library *library = data.getLibrariesMap().value(normalUser->getLibraryId(), nullptr);
    if (library == nullptr) {
        outcome.failReason = "Library not found";
        return outcome;
    }

    Wallet *buyerWallet = data.getWalletsMap().value(buyer->getWalletId(), nullptr);
    if (buyerWallet == nullptr) {
        outcome.failReason = "Wallet not found";
        return outcome;
    }

    if (bookIds.isEmpty()) {
        outcome.failReason = "No books to purchase";
        return outcome;
    }

    qint64 totalAmount = 0;
    for (quint64 bookId: bookIds) {
        Book *book = data.getBooksMap().value(bookId, nullptr);
        if (book == nullptr || !book->getIsActive()) {
            outcome.failReason = "One of the books is unavailable";
            return outcome;
        }

        User *publisherUser = data.getUsersMap().value(book->getPublisherId(), nullptr);
        if (publisherUser == nullptr || publisherUser->getRole() != Role::PUBLISHER) {
            outcome.failReason = "One of the books has no valid publisher";
            return outcome;
        }

        Wallet *publisherWallet = data.getWalletsMap().value(publisherUser->getWalletId(), nullptr);
        if (publisherWallet == nullptr) {
            outcome.failReason = "One of the publishers has no valid wallet";
            return outcome;
        }

        totalAmount += book->getFinalPrice();
    }

    if (buyerWallet->getBalance() < totalAmount) {
        outcome.failReason = "Insufficient balance";
        return outcome;
    }

    buyerWallet->withdraw(totalAmount);

    Transaction *buyerTransaction = new Transaction();
    buyerTransaction->assignNewId();
    buyerTransaction->setWalletId(buyerWallet->getId());
    buyerTransaction->setAmount(totalAmount);
    buyerTransaction->setType(TransactionType::PURCHASE);
    data.getTransactionsMap().insert(buyerTransaction->getId(), buyerTransaction);
    buyerWallet->addTransaction(buyerTransaction->getId());

    for (quint64 bookId: bookIds) {
        Book *book = data.getBooksMap().value(bookId, nullptr);
        User *publisherUser = data.getUsersMap().value(book->getPublisherId(), nullptr);
        Publisher *publisher = static_cast<Publisher *>(publisherUser);
        Wallet *publisherWallet = data.getWalletsMap().value(publisherUser->getWalletId(), nullptr);

        qint64 bookPrice = book->getFinalPrice();
        publisherWallet->deposit(bookPrice);
        publisher->receiveSaleIncome(bookPrice);
        book->incrementSales();

        Transaction *saleTransaction = new Transaction();
        saleTransaction->assignNewId();
        saleTransaction->setWalletId(publisherWallet->getId());
        saleTransaction->setAmount(bookPrice);
        saleTransaction->setType(TransactionType::SALE_INCOME);
        saleTransaction->setBookPriceAtPurchase(bookId, bookPrice);
        data.getTransactionsMap().insert(saleTransaction->getId(), saleTransaction);
        publisherWallet->addTransaction(saleTransaction->getId());

        library->addToPurchasedBooks(bookId);
    }

    Purchase *purchase = new Purchase();
    purchase->assignNewId();
    purchase->setBuyerId(buyerId);
    purchase->setBookIds(bookIds);
    purchase->setTotalAmount(totalAmount);
    data.getPurchasesMap().insert(purchase->getId(), purchase);


    pushNotification(buyerId, NotificationType::PURCHASE_COMPLETE, "خرید شما با موفقیت انجام شد");

    QSet<quint64> notifiedPublishers;
    for (quint64 bookId: bookIds) {
        Book *book = data.getBooksMap().value(bookId, nullptr);
        if (book == nullptr || notifiedPublishers.contains(book->getPublisherId())) {
            continue;
        }
        notifiedPublishers.insert(book->getPublisherId());
        pushNotification(book->getPublisherId(), NotificationType::NEW_SALE_FOR_PUBLISHER,
                         "یکی از کتاب‌های شما فروخته شد");
    }

    outcome.success = true;
    outcome.purchaseId = purchase->getId();
    outcome.totalAmount = totalAmount;
    return outcome;
}

void ServerCore::handleBuyRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    QSet<quint64> bookIds;
    QStringList idParts = fields.at(0).split(",");
    for (int i = 0; i < idParts.size(); i++) {
        if (!idParts.at(i).isEmpty()) {
            bookIds.insert(idParts.at(i).toULongLong());
        }
    }

    PurchaseOutcome outcome = performPurchase(userId, bookIds);
    if (!outcome.success) {
        handler->sendResponse(Command::FAIL, {outcome.failReason});
        return;
    }

    handler->sendResponse(Command::SUCCESS, {
                              QString::number(outcome.purchaseId), QString::number(outcome.totalAmount)
                          });
}

void ServerCore::handleGetCartItemsRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    Cart *cart = findOrCreateCartForUser(userId);
    QSet<quint64> bookIds = cart->getBookIds();

    if (bookIds.isEmpty()) {
        handler->sendResponse(Command::CART_RESULT, {"EMPTY"});
        return;
    }

    QStringList idsList;
    for (quint64 bookId: bookIds) {
        idsList.append(QString::number(bookId));
    }

    handler->sendResponse(Command::CART_RESULT, {QString::number(idsList.size()), idsList.join(",")});
}

void ServerCore::handleGetCartBookSummaryRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    QString authorName = getAuthorName(book->getAuthorId());
    QString imageBase64 = getCoverImageOrPlaceholder(book->getCoverImagePath());

    handler->sendResponse(Command::CART_BOOK_SUMMARY, {
                              QString::number(book->getId()),
                              imageBase64,
                              book->getTitle(),
                              authorName,
                              QString::number(book->getPrice()),
                              QString::number((long long)(book->getPrice() * book->getDiscountPercent()))
                          });
}

void ServerCore::handleAddCartRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();
    Book *book = data.getBooksMap().value(bookId, nullptr);
    if (book == nullptr) {
        handler->sendResponse(Command::FAIL, {"Book not found"});
        return;
    }

    Cart *cart = findOrCreateCartForUser(userId);
    cart->addItem(bookId);

    handler->sendResponse(Command::SUCCESS, {"Added to cart"});
}

void ServerCore::handleRemoveCartRequest(ClientHandler *handler, const QStringList &fields) {
    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    if (fields.size() < 1) {
        handler->sendResponse(Command::FAIL, {"Invalid data"});
        return;
    }

    quint64 bookId = fields.at(0).toULongLong();

    Cart *cart = findOrCreateCartForUser(userId);
    cart->removeItem(bookId);

    handler->sendResponse(Command::SUCCESS, {"Removed from cart"});
}

void ServerCore::handleCheckoutCartRequest(ClientHandler *handler, const QStringList &fields) {
    Q_UNUSED(fields);

    quint64 userId = requireAuthentication(handler);
    if (userId == 0) {
        return;
    }

    Cart *cart = findOrCreateCartForUser(userId);
    QSet<quint64> bookIds = cart->getBookIds();

    PurchaseOutcome outcome = performPurchase(userId, bookIds);
    if (!outcome.success) {
        handler->sendResponse(Command::CHECKOUT_RESULT, {"FAILED"});
        return;
    }

    cart->clear();
    handler->sendResponse(Command::CHECKOUT_RESULT, {"SUCCESS"});
}

//in the name of ALLAH
//YA MAHDI

#include "SearchEngine.h"

SearchEngine::SearchEngine() {
    titleTrieRoot = new TrieNode();
}

SearchEngine::~SearchEngine() {
    delete titleTrieRoot;
    titleTrieRoot = nullptr;
}

void SearchEngine::buildIndexes(const QMap<quint64, Book *> &books) {
    QList<quint64> keys = books.keys();
    for (quint64 key : keys) {
        Book *book = books.value(key);
        addBookToIndex(book);
    }
}

void SearchEngine::addBookToIndex(const Book *book) {
    if (book == nullptr) {
        return;
    }

    quint64 bookId = book->getId();
    QString lowerTitle = book->getTitle().toLower();

    titleTrieRoot->insert(lowerTitle, bookId);
    bookTitleLookup.insert(bookId, lowerTitle);

    bookGenreLookup.insert(bookId, book->getGenre());
    genreIndex[book->getGenre()].insert(bookId);

    bookAuthorLookup.insert(bookId, book->getAuthorId());
    authorIndex[book->getAuthorId()].insert(bookId);

    bookPublisherLookup.insert(bookId, book->getPublisherId());
    publisherIndex[book->getPublisherId()].insert(bookId);
}

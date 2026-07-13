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

void SearchEngine::removeBookFromIndex(quint64 bookId) {
    if (!bookTitleLookup.contains(bookId)) {
        return;
    }

    QString title = bookTitleLookup.value(bookId);
    titleTrieRoot->removeBookId(title, bookId);
    bookTitleLookup.remove(bookId);

    Genre genre = bookGenreLookup.value(bookId);
    genreIndex[genre].remove(bookId);
    bookGenreLookup.remove(bookId);

    quint64 authorId = bookAuthorLookup.value(bookId);
    authorIndex[authorId].remove(bookId);
    bookAuthorLookup.remove(bookId);

    quint64 publisherId = bookPublisherLookup.value(bookId);
    publisherIndex[publisherId].remove(bookId);
    bookPublisherLookup.remove(bookId);
}

QSet<quint64> SearchEngine::searchByTitle(const QString &prefix) const {
    return titleTrieRoot->searchPrefix(prefix.toLower());
}

QSet<quint64> SearchEngine::searchByAuthor(quint64 authorId) {
    return authorIndex.value(authorId);
}

QSet<quint64> SearchEngine::searchByPublisher(quint64 publisherId) const {
    return publisherIndex.value(publisherId);
}

QSet<quint64> SearchEngine::filterByGenre(Genre genre) {
    return genreIndex.value(genre);
}

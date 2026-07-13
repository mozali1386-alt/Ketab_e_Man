//in the name of ALLAH
//YA MAHDI

#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QMap>
#include <QSet>
#include "TrieNode.h"
#include "../../shared/Book.h"
#include "../../shared/Enums.h"

class SearchEngine {
public:
    SearchEngine();

    ~SearchEngine();

    void buildIndexes(const QMap<quint64, Book *> &books);

    void addBookToIndex(const Book *book);

    void removeBookFromIndex(quint64 bookId);

    QSet<quint64> searchByTitle(const QString &prefix) const;

    QSet<quint64> searchByAuthor(quint64 authorId);

    QSet<quint64> searchByPublisher(quint64 publisherId) const;

    QSet<quint64> filterByGenre(Genre genre);

private:
    QMap<Genre, QSet<quint64> > genreIndex;
    QMap<quint64, QSet<quint64> > authorIndex;
    QMap<quint64, QSet<quint64> > publisherIndex;

    QMap<quint64, QString> bookTitleLookup;
    QMap<quint64, Genre> bookGenreLookup;
    QMap<quint64, quint64> bookAuthorLookup;
    QMap<quint64, quint64> bookPublisherLookup;

    TrieNode *titleTrieRoot;
};

#endif

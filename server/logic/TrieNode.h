//in the name of ALLAH
//YA MAHDI

#ifndef TRIENODE_H
#define TRIENODE_H

#include <QMap>
#include <QSet>
#include <QChar>
#include <QString>

class TrieNode {
public:
    TrieNode();

    ~TrieNode();

    void insert(const QString &word, quint64 bookId);

    QSet<quint64> searchPrefix(const QString &prefix);

    void removeBookId(const QString &word, quint64 bookId);

private:
    QMap<QChar, TrieNode *> children;
    bool isEndOfWord;
    QSet<quint64> bookIds;
};

#endif

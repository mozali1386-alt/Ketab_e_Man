//in the name of ALLAH
//YA MAHDI

#include "TrieNode.h"

TrieNode::TrieNode() {
    isEndOfWord = false;
}

TrieNode::~TrieNode() {
    QList<QChar> keys = children.keys();
    for (auto key: keys) {
        TrieNode *child = children.value(key);
        delete child;
    }
    children.clear();
}

void TrieNode::insert(const QString &word, quint64 bookId) {
    TrieNode *current = this;

    for (int i = 0; i < word.length(); i++) {
        QChar ch = word.at(i);

        if (!current->children.contains(ch)) {
            auto newNode = new TrieNode();
            current->children.insert(ch, newNode);
        }

        current = current->children.value(ch);
        current->bookIds.insert(bookId);
    }

    current->isEndOfWord = true;
}

QSet<quint64> TrieNode::searchPrefix(const QString &prefix) {
    TrieNode *current = this;

    for (int i = 0; i < prefix.length(); i++) {
        QChar ch = prefix.at(i);

        if (!current->children.contains(ch)) {
            QSet<quint64> empty;
            return empty;
        }

        current = current->children.value(ch);
    }

    return current->bookIds;
}

void TrieNode::removeBookId(const QString &word, quint64 bookId) {
    TrieNode *current = this;

    for (int i = 0; i < word.length(); i++) {
        QChar ch = word.at(i);

        if (!current->children.contains(ch)) {
            return;
        }

        current = current->children.value(ch);
        current->bookIds.remove(bookId);
    }
}

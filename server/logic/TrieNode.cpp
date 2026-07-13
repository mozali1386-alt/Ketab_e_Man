//in the name of ALLAH
//YA MAHDI

#include "TrieNode.h"
#include <QVector>

TrieNode::TrieNode() {
    isEndOfWord = false;
}

TrieNode::~TrieNode() {
    QList<QChar> keys = children.keys();
    for (int i = 0; i < keys.size(); i++) {
        TrieNode *child = children.value(keys.at(i));
        delete child;
    }
    children.clear();
}

void TrieNode::insert(const QString &word, quint64 bookId) {
    TrieNode *current = this;

    for (int i = 0; i < word.length(); i++) {
        QChar ch = word.at(i);

        if (!current->children.contains(ch)) {
            TrieNode *newNode = new TrieNode();
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
    QVector<TrieNode *> pathNodes;
    QVector<QChar> pathChars;

    TrieNode *current = this;
    pathNodes.append(current);

    for (int i = 0; i < word.length(); i++) {
        QChar ch = word.at(i);

        if (!current->children.contains(ch)) {
            return;
        }

        current = current->children.value(ch);
        current->bookIds.remove(bookId);

        pathNodes.append(current);
        pathChars.append(ch);
    }

    if (current->bookIds.isEmpty()) {
        current->isEndOfWord = false;
    }

    for (int i = pathNodes.size() - 1; i >= 1; i--) {
        TrieNode *node = pathNodes.at(i);

        bool nodeIsDead = node->bookIds.isEmpty() && node->children.isEmpty() && !node->isEndOfWord;

        if (!nodeIsDead) {
            break;
        }

        TrieNode *parent = pathNodes.at(i - 1);
        QChar charToParent = pathChars.at(i - 1);

        parent->children.remove(charToParent);
        delete node;
    }
}

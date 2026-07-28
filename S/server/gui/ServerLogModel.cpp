//in the name of ALLAH
//YA MAHDI

#include "ServerLogModel.h"
#include <QDateTime>

ServerLogModel::ServerLogModel(QObject *parent) : QAbstractListModel(parent) {
}

int ServerLogModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return logLines.size();
}

QVariant ServerLogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= logLines.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return logLines.at(index.row());
    }

    return QVariant();
}

void ServerLogModel::addLogMessage(QString message) {
    QString timeStamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString finalLine = "[" + timeStamp + "] " + message;

    if (logLines.size() >= MAX_LOG_LINES) {
        beginRemoveRows(QModelIndex(), 0, 0);
        logLines.remove(0);
        endRemoveRows();
    }

    int newRow = logLines.size();
    beginInsertRows(QModelIndex(), newRow, newRow);
    logLines.append(finalLine);
    endInsertRows();
}

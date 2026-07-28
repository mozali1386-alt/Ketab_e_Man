//in the name of ALLAH
//YA MAHDI

#ifndef SERVERLOGMODEL_H
#define SERVERLOGMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QString>

class ServerLogModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit ServerLogModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void addLogMessage(QString message);

private:
    QVector<QString> logLines;
    static const int MAX_LOG_LINES = 500;
};

#endif

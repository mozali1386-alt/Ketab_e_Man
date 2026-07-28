#ifndef STOREPAGE_H
#define STOREPAGE_H

#include <QString>
#include <QWidget>
#include "clientsocketmanager.h"

namespace Ui {
class Storepage;
}

class Storepage : public QWidget
{
    Q_OBJECT

public:
    explicit Storepage(QWidget *parent = nullptr);
    ~Storepage();
    void refreshStore();
    void setClient(ClientSocketManager *client);

private slots:

    void on_pushButton_search_clicked();

private:
    Ui::Storepage *ui;

    QString lastsearchdisplay;
    void processServerResponse(const QString &response);
    void requestBookSummary(const QString &bookId);
    ClientSocketManager *m_client = nullptr;
};

#endif // STOREPAGE_H
#ifndef PUBLISHERPROFILEPAGE_H
#define PUBLISHERPROFILEPAGE_H

#include <QString>
#include <QWidget>
#include "clientsocketmanager.h"

namespace Ui {
class PublisherProfilePage;
}

class PublisherProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherProfilePage(QWidget *parent = nullptr);
    ~PublisherProfilePage();
    void refreshProfile();
    void setClient(ClientSocketManager *client);

private slots:
    void on_pushButton_save_clicked();
    void on_pushButton_enseraf_clicked();
    void on_pushButton_withdrawbalance_clicked();
    void on_pushButton_editusername_clicked();
    void processServerResponse(const QString &response);

private:
    Ui::PublisherProfilePage *ui;
    QString originalEntesharat;
    QString originalEmail;
    QString originalExplanation;
    ClientSocketManager *m_client = nullptr;
};

#endif // PUBLISHERPROFILEPAGE_H
#ifndef SIGNINWINDOW_H
#define SIGNINWINDOW_H

#include <QMainWindow>
#include "clientsocketmanager.h"

namespace Ui {
class signinwindow;
}

class signinwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit signinwindow(ClientSocketManager *client,
                          const QString &role,
                          QWidget *parent = nullptr);
    ~signinwindow();

signals:
    void backtologinwindow();
    void signinsuccessful();

private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_Confirm_clicked();

    void on_pushButton_forgetpassword_clicked();

    void processServerResponse(const QString &response);

private:
    Ui::signinwindow *ui;
    QString role;
    ClientSocketManager *m_client;
};

#endif // SIGNINWINDOW_H

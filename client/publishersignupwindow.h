#ifndef PUBLISHERSIGNUPWINDOW_H
#define PUBLISHERSIGNUPWINDOW_H

#include <QMainWindow>
#include "clientsocketmanager.h"

namespace Ui {
class publishersignupwindow;
}

class publishersignupwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit publishersignupwindow(ClientSocketManager *client, QWidget *parent = nullptr);
    ~publishersignupwindow();

signals:
    void backtologinwindow();
    void signupsuccessful();

private slots:
    void on_pushButton_back_clicked();

    void on_pushButton_confirm_clicked();
    void processServerResponse(const QString &response);

private:
    Ui::publishersignupwindow *ui;
    ClientSocketManager *m_client;
};

#endif // PUBLISHERSIGNUPWINDOW_H

#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "clientsocketmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginWindow;
}
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(ClientSocketManager *client, QWidget *parent = nullptr);
    ~LoginWindow() override;

private slots:
    void on_pushButton_signup_clicked();

    void on_pushButton_signin_clicked();

private:
    Ui::LoginWindow *ui;
    ClientSocketManager *m_client;
};
#endif

#ifndef RESETPASSWORDDIALOG_H
#define RESETPASSWORDDIALOG_H

#include <QDialog>
#include "clientsocketmanager.h"

namespace Ui {
class ResetPasswordDialog;
}

class ResetPasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetPasswordDialog(ClientSocketManager *client, QWidget *parent = nullptr);
    ~ResetPasswordDialog();

    void setupForProfile();
private slots:
    void on_pushButton_page1back_clicked();

    void on_pushButton_page1next_clicked();

    void on_pushButton_page2back_2_clicked();

    void on_pushButton_page2next_2_clicked();
    void processServerResponse(const QString &response);

private:
    Ui::ResetPasswordDialog *ui;
    bool isFromProfile = false;
    ClientSocketManager *m_client;
};

#endif // RESETPASSWORDDIALOG_H

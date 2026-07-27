#ifndef PUBLISHERDASHBOARD_H
#define PUBLISHERDASHBOARD_H

#include <QLabel>
#include <QMainWindow>
#include "notification.h"

namespace Ui {
class Publisherdashboard;
}

class Publisherdashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Publisherdashboard(QWidget *parent = nullptr);
    ~Publisherdashboard();

private slots:
    void on_pushButton_managmentbook_clicked();

    void on_pushButton_statistic_clicked();

    void on_pushButton_hesabkarbar_clicked();

    void on_toolButton_notification_clicked();
    void onNotificationClicked(const QString &notifId);
    void onMarkAllReadClicked();

private:
    Ui::Publisherdashboard *ui;

    Notification *notifPopup;
    QLabel *badgeLabel;
    int unreadCount = 0;

    void updateBadge();
    void processServerResponse(const QString &response);
};

#endif // PUBLISHERDASHBOARD_H

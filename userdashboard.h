#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QCloseEvent>
#include <QLabel>
#include <QMainWindow>
#include "notification.h"

namespace Ui {
class UserDashboard;
}

class UserDashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserDashboard(QWidget *parent = nullptr);
    ~UserDashboard();

protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void on_pushButton_store_clicked();

    void on_pushButton_cart_clicked();

    void on_pushButton_mylibrary_clicked();

    void on_pushButton_profile_clicked();

    void on_toolButton_notification_clicked();
    void onNotificationClicked(const QString &notifId);
    void onMarkAllReadClicked();

private:
    Ui::UserDashboard *ui;

    Notification *notifPopup;
    QLabel *badgeLabel; // لیبل دایره قرمز برای نمایش عدد
    int unreadCount = 0;

    void updateBadge(); // تابعی برای روشن/خاموش کردن دایره قرمز
    void processServerResponse(const QString &response);
};

#endif // USERDASHBOARD_H

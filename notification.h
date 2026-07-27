#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>

class Notification : public QDialog
{
    Q_OBJECT
public:
    explicit Notification(QWidget *parent = nullptr);

    // تابعی برای اضافه کردن یک اعلان جدید به لیست
    void addNotification(const QString &id, const QString &text, const QString &date, bool isRead);

    // تابعی برای پاک کردن کل لیست
    void clearNotifications();
    // تابعی برای خاکستری کردن تمام دایره‌ها (بدون حذف پیام‌ها)
    void markAllAsReadUI();
    // تابعی برای خاکستری کردن دایره فقط برای یک اعلان خاص
    void markAsReadUI(const QString &id);

signals:
    void notificationClicked(const QString &id);
    void markAllReadClicked();

private:
    QListWidget *listWidget;
    QPushButton *markAllReadBtn;
};

#endif // NOTIFICATION_H
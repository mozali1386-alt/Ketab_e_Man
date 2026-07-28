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
    void addNotification(const QString &id, const QString &text, const QString &date, bool isRead);
    void clearNotifications();

    void markAllAsReadUI();
    void markAsReadUI(const QString &id);

signals:
    void notificationClicked(const QString &id);
    void markAllReadClicked();

private:
    QListWidget *listWidget;
    QPushButton *markAllReadBtn;
};

#endif
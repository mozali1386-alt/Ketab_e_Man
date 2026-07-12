#ifndef ADMINDASHBOARD_H
#define ADMINDASHBOARD_H

#include <QMainWindow> // ارث‌بری از QMainWindow کاملاً درست است
#include <QQuickWidget>

namespace Ui {
class Admindashboard;
}

class Admindashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Admindashboard(QWidget *parent = nullptr);
    ~Admindashboard();

    // این توابع را گذاشتم تا بعداً در کد خودت هر وقت دیتای واقعی گرفتی صدایشان بزنی
    void setOnlineUsers(int count);
    void setTotalUsers(int count);
    void setNotificationCount(int count);

private slots:
    void onBellClicked(); // اسلات زنگوله برای باز کردن صفحه اعلان‌ها توسط خودت

private:
    Ui::Admindashboard *ui;
};

#endif // ADMINDASHBOARD_H
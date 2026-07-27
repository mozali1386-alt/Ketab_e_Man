#include "publisherdashboard.h"
#include <QTimer>
#include "ui_publisherdashboard.h"

Publisherdashboard::Publisherdashboard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Publisherdashboard)
{
    ui->setupUi(this);
    ui->statusbar->setStyleSheet("color: red; font-weight: bold;");

    notifPopup = new Notification(this);
    connect(notifPopup,
            &Notification::notificationClicked,
            this,
            &Publisherdashboard::onNotificationClicked);
    connect(notifPopup,
            &Notification::markAllReadClicked,
            this,
            &Publisherdashboard::onMarkAllReadClicked);

    badgeLabel = new QLabel(ui->toolButton_notification);

    badgeLabel->setObjectName("redNotificationBadge");
    badgeLabel->setFixedSize(14, 14);

    badgeLabel->setStyleSheet(
        "QLabel#redNotificationBadge {"
        "   background-color: #e74c3c;"
        "   color: white;"
        "   border-radius: 7px;"
        "   font-size: 9px;"
        "   font-weight: bold;"
        "   border-image: none;" /* <--- این خط عکس زنگوله را از روی دایره پاک می‌کند */
        "   background-image: none;" /* <--- برای اطمینان بیشتر */
        "   border: none;"
        "}");

    badgeLabel->setAlignment(Qt::AlignCenter);
    badgeLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    badgeLabel->move(15, 0);
    badgeLabel->raise();
    badgeLabel->hide(); // در ابتدا مخفی است
    ui->stackedWidget->setCurrentWidget(ui->page_managementbook);

    ui->pushButton_managmentbook->setChecked(true);

    // ==========================================================
    // === شروع کدهای شبیه‌سازی سرور (بعد از وصل شدن به سرور پاک شوند) ===
    // ==========================================================

    // ۱. تست پیام‌های آفلاین (قدیمی) - فقط در پاپ‌آپ می‌آیند (بدون استاتوس‌بار)
    QTimer::singleShot(2000, this, [=]() {
        // آفلاین ۱: خوانده شده (زنگوله تغییر نمی‌کند)
        processServerResponse("NOTIFICATION_INFO_RESULT||101||کتاب 'بوف کور' نوشته صادق هدایت، به "
                              "کتابخانه شما اضافه شد.||14:30||READ");

        // آفلاین ۲: نخوانده (زنگوله ۱ عدد بالا می‌رود)
        processServerResponse("NOTIFICATION_INFO_RESULT||102||کتاب جدیدی در ژانر کلاسیک که به آن "
                              "علاقه دارید، در فروشگاه موجود شد!||15:45||UNREAD");
    });

    // ۲. تست پیام‌های زنده (Real-Time) - همراه با استاتوس‌بار و صدا
    QTimer::singleShot(5000, this, [=]() {
        // زنده ۱: نخوانده (استاتوس‌بار پایین قرمز می‌شود + زنگوله ۱ عدد بالا می‌رود)
        processServerResponse("NEW_NOTIFICATION_PUSH||103||موجودی حساب شما با موفقیت شارژ شد. "
                              "هم‌اکنون می‌توانید "
                              "خرید خود را نهایی کنید.||16:20||UNREAD");
    });

    QTimer::singleShot(8000, this, [=]() {
        // زنده ۲: نخوانده (استاتوس‌بار پایین قرمز می‌شود + زنگوله ۱ عدد بالا می‌رود)
        processServerResponse("NEW_NOTIFICATION_PUSH||104||تخفیف ویژه ۵۰ درصدی فقط تا پایان امشب "
                              "برای شما فعال شد!||16:25||UNREAD");
    });

    // ==========================================================
    // === پایان کدهای شبیه‌سازی ===
    // ==========================================================
}

Publisherdashboard::~Publisherdashboard()
{
    delete ui;
}

void Publisherdashboard::on_pushButton_managmentbook_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_managementbook);
    ui->page_managementbook->refreshBooksList();
}

void Publisherdashboard::on_pushButton_statistic_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_statistic);
    ui->scrollAreaWidgetContents->refreshStatistics();
}

void Publisherdashboard::on_pushButton_hesabkarbar_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_profile);
    ui->page_profile->refreshProfile();
}
void Publisherdashboard::updateBadge()
{
    if (unreadCount > 0) {
        badgeLabel->setText(QString::number(unreadCount));
        badgeLabel->show();
    } else {
        badgeLabel->hide();
    }
}

void Publisherdashboard::on_toolButton_notification_clicked()
{
    if (notifPopup->isVisible()) {
        notifPopup->close();
        return;
    }
    QPoint pos = ui->toolButton_notification->mapToGlobal(
        QPoint(0, ui->toolButton_notification->height()));
    notifPopup->move(pos);
    notifPopup->show();
}

void Publisherdashboard::onNotificationClicked(const QString &notifId)
{
    QString req = "MARK_NOTIFICATION_READ||" + notifId;
    // client->sendMessage(req);

    // ==========================================================
    // === شروع کدهای شبیه‌سازی (بعد از اتصال به سرور پاک شوند) ===
    // ==========================================================
    QTimer::singleShot(100, this, [=]() {
        // سرور جواب می‌دهد که پیام با موفقیت خوانده شد
        processServerResponse("MARK_READ_RESULT||" + notifId + "||SUCCESS");
    });
    // ==========================================================
}

void Publisherdashboard::onMarkAllReadClicked()
{
    QString req = "MARK_ALL_NOTIFICATIONS_READ";
    // client->sendMessage(req);

    // ==========================================================
    // === شروع کدهای شبیه‌سازی (بعد از اتصال به سرور پاک شوند) ===
    // ==========================================================
    QTimer::singleShot(100, this, [=]() {
        // سرور جواب می‌دهد که همه پیام‌ها با موفقیت خوانده شدند
        processServerResponse("MARK_ALL_READ_RESULT||SUCCESS");
    });
    // ==========================================================
}
void Publisherdashboard::processServerResponse(const QString &response)
{
    const QStringList parts = response.split("||", Qt::KeepEmptyParts);
    if (parts.isEmpty())
        return;
    QString cmd = parts[0];

    if (cmd == "NOTIFICATION_INFO_RESULT") {
        if (parts.size() >= 5) {
            QString notifId = parts[1];
            QString msg = parts[2];
            QString date = parts[3];
            QString status = parts[4];
            bool isRead = (status == "READ");

            if (notifPopup)
                notifPopup->addNotification(notifId, msg, date, isRead);

            if (!isRead) {
                unreadCount++;
                updateBadge();
            }
        }
    } else if (cmd == "NEW_NOTIFICATION_PUSH") {
        if (parts.size() >= 5) {
            QString notifId = parts[1];
            QString msg = parts[2];
            QString date = parts[3];
            QString status = parts[4];
            bool isRead = (status == "READ");

            if (notifPopup)
                notifPopup->addNotification(notifId, msg, date, isRead);

            if (!isRead) {
                unreadCount++;
                updateBadge();

                // نمایش در پایین صفحه به مدت ۵ ثانیه
                ui->statusbar->showMessage("اعلان جدید: " + msg, 5000);
                QApplication::beep();
            }
        }
    } else if (cmd == "MARK_READ_RESULT") {
        if (parts.size() >= 3 && parts[2] == "SUCCESS") {
            QString notifId = parts[1];
            if (unreadCount > 0) {
                unreadCount--;
                updateBadge();
            }
            if (notifPopup) {
                notifPopup->markAsReadUI(notifId);
            }
        }
    } else if (cmd == "MARK_ALL_READ_RESULT") {
        if (parts.size() >= 2 && parts[1] == "SUCCESS") {
            unreadCount = 0;
            updateBadge();

            if (notifPopup) {
                notifPopup->markAllAsReadUI();
            }
        }
    }
}

void Publisherdashboard::closeEvent(QCloseEvent *event)
{
    // client->sendMessage("LOGOUT");
    // اجازه می‌دهیم برنامه روال عادی بسته شدن خود را طی کند
    event->accept();
}
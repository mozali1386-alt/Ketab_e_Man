#include "publisherdashboard.h"
#include <QTimer>
#include "ui_publisherdashboard.h"

Publisherdashboard::Publisherdashboard(ClientSocketManager *client, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Publisherdashboard)
    , m_client(client)
{
    ui->setupUi(this);
    ui->page_managementbook->setClient(m_client);
    // برای اینکه تابع رفرش مدیریت کتاب صدا زده شود
    on_pushButton_managmentbook_clicked();
    ui->scrollAreaWidgetContents->setClient(m_client);
    ui->page_profile->setClient(m_client);
    connect(m_client,
            &ClientSocketManager::messageReceived,
            this,
            &Publisherdashboard::processServerResponse);

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
    m_client->sendMessage(req);
}

void Publisherdashboard::onMarkAllReadClicked()
{
    QString req = "MARK_ALL_NOTIFICATIONS_READ";
    m_client->sendMessage(req);
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
    m_client->sendMessage("LOGOUT");
    // اجازه می‌دهیم برنامه روال عادی بسته شدن خود را طی کند
    event->accept();
}
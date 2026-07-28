#include "notification.h"
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QVBoxLayout>

Notification::Notification(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setFixedSize(350, 450);
    setAttribute(Qt::WA_TranslucentBackground);

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setObjectName("mainWidget");
    mainWidget->setStyleSheet(
        "#mainWidget { background-color: white; border-radius: 12px; border: 1px solid #dcdde1; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 50));
    shadow->setOffset(0, 5);
    mainWidget->setGraphicsEffect(shadow);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(mainWidget);

    QVBoxLayout *innerLayout = new QVBoxLayout(mainWidget);
    innerLayout->setContentsMargins(15, 15, 15, 15);
    innerLayout->setSpacing(10);

    QLabel *titleLabel = new QLabel("اعلان‌های سیستم");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2f3640;");
    innerLayout->addWidget(titleLabel);

    listWidget = new QListWidget();
    listWidget->setStyleSheet(
        "QListWidget { border: none; background: transparent; outline: none; }"
        "QListWidget::item { border-bottom: 1px solid #f5f6fa; padding: 5px; }"
        "QListWidget::item:hover { background-color: #f1f2f6; border-radius: 8px; }"
        "QScrollBar:vertical { width: 8px; background: transparent; }"
        "QScrollBar::handle:vertical { background: #dcdde1; border-radius: 4px; }");
    innerLayout->addWidget(listWidget);

    markAllReadBtn = new QPushButton(
        "علامت‌گذاری همه به عنوان خوانده شده");
    markAllReadBtn->setCursor(Qt::PointingHandCursor);
    markAllReadBtn->setStyleSheet(
        "QPushButton { background-color: #f5f6fa; color: #0097e6; border: none; border-radius: "
        "6px; padding: 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #dcdde1; }");
    innerLayout->addWidget(markAllReadBtn);

    connect(markAllReadBtn, &QPushButton::clicked, this, &Notification::markAllReadClicked);
    connect(listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        QString notifId = item->data(Qt::UserRole).toString();
        emit notificationClicked(notifId);
    });
}

void Notification::addNotification(const QString &id,
                                   const QString &text,
                                   const QString &date,
                                   bool isRead)
{
    QListWidgetItem *listItem = new QListWidgetItem(listWidget);
    listItem->setData(Qt::UserRole, id);

    QWidget *rowWidget = new QWidget();
    // ارتفاع را کمتر کردیم (65) تا جمع‌وجورتر شود
    rowWidget->setFixedSize(305, 65);

    QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
    rowLayout->setContentsMargins(10, 5, 20, 5);
    rowLayout->setSpacing(12);
    rowLayout->setDirection(QBoxLayout::RightToLeft);

    QLabel *dotLabel = new QLabel();
    dotLabel->setObjectName("dotLabel"); // <--- این خط حتماً اضافه شود
    dotLabel->setFixedSize(12, 12);
    QString dotColor = isRead ? "#b2bec3" : "#e84118";
    dotLabel->setStyleSheet(QString("background-color: %1; border-radius: 6px;").arg(dotColor));

    QLabel *textLabel = new QLabel(text);
    textLabel->setWordWrap(true);
    textLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    textLabel->setStyleSheet(
        "color: #2f3640; font-size: 12px;"); // فونت را کمی کوچکتر کردم تا متن طولانی جا شود

    QLabel *dateLabel = new QLabel(date);
    dateLabel->setStyleSheet("color: #7f8fa6; font-size: 11px;");
    dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);

    rowLayout->addWidget(dotLabel, 0, Qt::AlignTop);
    rowLayout->addWidget(textLabel, 1, Qt::AlignTop);
    rowLayout->addWidget(dateLabel, 0, Qt::AlignBottom);

    listItem->setSizeHint(QSize(305, 65));
    listWidget->setItemWidget(listItem, rowWidget);
}
void Notification::clearNotifications()
{
    listWidget->clear();
}
void Notification::markAllAsReadUI()
{
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        QWidget *widget = listWidget->itemWidget(item);
        if (widget) {
            QLabel *dot = widget->findChild<QLabel *>("dotLabel");
            if (dot) {
                dot->setStyleSheet("background-color: #b2bec3; border-radius: 6px;");
            }
        }
    }
}
void Notification::markAsReadUI(const QString &id)
{
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item->data(Qt::UserRole).toString() == id) { // اگر آیدی پیام پیدا شد
            QWidget *widget = listWidget->itemWidget(item);
            if (widget) {
                QLabel *dot = widget->findChild<QLabel *>("dotLabel");
                if (dot) {
                    dot->setStyleSheet("background-color: #b2bec3; border-radius: 6px;");
                }
            }
            break;
        }
    }
}
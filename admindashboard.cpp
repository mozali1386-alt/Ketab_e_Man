#include "admindashboard.h"
#include <QDebug>
#include <QQuickItem>
#include "ui_admindashboard.h"

Admindashboard::Admindashboard(QWidget *parent)
    : QMainWindow(parent)
    , // مقداردهی اولیه به عنوان QMainWindow
    ui(new Ui::Admindashboard)
{
    ui->setupUi(this);
    // ۱. بارگذاری فایل QML داخل ویجت
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/AdminStats.qml")));
    ui->quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->quickWidget->setClearColor(Qt::transparent);

    // ۲. دسترسی به ریشه QML و مقداردهی اولیه روی صفر
    QObject *qmlRoot = ui->quickWidget->rootObject();
    if (qmlRoot) {
        // تنظیم مقادیر پیش‌فرض روی صفر
        qmlRoot->setProperty("onlineUsers", 0);
        qmlRoot->setProperty("totalUsers", 0);
        qmlRoot->setProperty("notificationCount", 0);

        // اتصال سیگنال کلیک زنگوله به سی‌پلاس‌پلاس
        connect(qmlRoot, SIGNAL(bellClicked()), this, SLOT(onBellClicked()));
    }
    // برای اینکه فیلد ستون های جدول بهتر نشان داده شوند
    ui->tableWidget_allusers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_allusers->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->tableWidget_books->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_books->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_comment->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_comment->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

Admindashboard::~Admindashboard()
{
    delete ui;
}

// توابعی که خودت بعداً برای پاس دادن مقادیر واقعی کلاینت-سرور استفاده می‌کنی:
void Admindashboard::setOnlineUsers(int count)
{
    QObject *qmlRoot = ui->quickWidget->rootObject();
    if (qmlRoot)
        qmlRoot->setProperty("onlineUsers", count);
}

void Admindashboard::setTotalUsers(int count)
{
    QObject *qmlRoot = ui->quickWidget->rootObject();
    if (qmlRoot)
        qmlRoot->setProperty("totalUsers", count);
}

void Admindashboard::setNotificationCount(int count)
{
    QObject *qmlRoot = ui->quickWidget->rootObject();
    if (qmlRoot)
        qmlRoot->setProperty("notificationCount", count);
}

// مدیریت کلیک روی زنگوله (باز کردن پنجره اعلان‌ها را خودت اینجا بنویس)
void Admindashboard::onBellClicked()
{
    qDebug() << "زنگوله کلیک شد!";

    // کدهای باز کردن بخش یا پنجره اعلان‌ها را اینجا اضافه کن:
    // مثلاً: ui->stackedWidget->setCurrentWidget(ui->pageNotifications);
}
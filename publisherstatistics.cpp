#include "publisherstatistics.h"
#include <QTimer> // اضافه شده برای کدهای تست
#include "ui_publisherstatistics.h"

PublisherStatistics::PublisherStatistics(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PublisherStatistics)
{
    ui->setupUi(this);

    ui->tableWidget_maximum->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_minimum->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

PublisherStatistics::~PublisherStatistics()
{
    delete ui;
}

// ==========================================
// ارسال درخواست‌ها به سرور (همراه با کدهای تست)
// ==========================================

void PublisherStatistics::requestGeneralStats()
{
    QString message = "GET_PUB_GENERAL_STATS";
    // client->sendMessage(message);

    // =============== شروع کدهای تست ===============
    QTimer::singleShot(100, this, [=]() {
        processServerResponse("PUB_GENERAL_RESULT||15||4.2||2500000");
    });
    // =============== پایان کدهای تست ===============
}

void PublisherStatistics::requestTopBooksIds()
{
    QString message = "GET_PUB_TOP_BOOKS_IDS";
    // client->sendMessage(message);

    // =============== شروع کدهای تست ===============
    QTimer::singleShot(150, this, [=]() {
        processServerResponse("PUB_TOP_IDS_RESULT||101,102,103");
    });
    // =============== پایان کدهای تست ===============
}

void PublisherStatistics::requestTopBookInfo(const QString &bookId)
{
    QString message = "GET_PUB_BOOK_INFO_TOP||" + bookId;
    // client->sendMessage(message);

    // =============== شروع کدهای تست ===============
    QTimer::singleShot(200, this, [=]() {
        QString mockResponse = "PUB_BOOK_INFO_TOP_RESULT||" + bookId + "||کتاب برتر " + bookId
                               + "||150||4.8";
        processServerResponse(mockResponse);
    });
    // =============== پایان کدهای تست ===============
}

void PublisherStatistics::requestLowestBooksIds()
{
    QString message = "GET_PUB_LOWEST_BOOKS_IDS";
    // client->sendMessage(message);

    // =============== شروع کدهای تست ===============
    QTimer::singleShot(150, this, [=]() {
        processServerResponse("PUB_LOWEST_IDS_RESULT||201,202");
    });
    // =============== پایان کدهای تست ===============
}

void PublisherStatistics::requestLowestBookInfo(const QString &bookId)
{
    QString message = "GET_PUB_BOOK_INFO_LOWEST||" + bookId;
    // client->sendMessage(message);

    // =============== شروع کدهای تست ===============
    QTimer::singleShot(200, this, [=]() {
        QString mockResponse = "PUB_BOOK_INFO_LOWEST_RESULT||" + bookId + "||کتاب کم فروش " + bookId
                               + "||5||2.1";
        processServerResponse(mockResponse);
    });
    // =============== پایان کدهای تست ===============
}

void PublisherStatistics::requestSalesChartData()
{
    QString message = "GET_PUB_SALES_CHART";
    // client->sendMessage(message);

    // =============== شروع کدهای تست ===============
    QTimer::singleShot(300, this, [=]() {
        processServerResponse("PUB_SALES_CHART_RESULT||کتاب الف:500,کتاب ب:350,کتاب ج:200,سایر "
                              "کتاب‌ها:150");
    });
    // =============== پایان کدهای تست ===============
}

void PublisherStatistics::requestScoreChartData()
{
    QString message = "GET_PUB_SCORE_CHART";
    // client->sendMessage(message);

    // =============== شروع کدهای تست ===============
    QTimer::singleShot(350, this, [=]() {
        processServerResponse("PUB_SCORE_CHART_RESULT||کتاب ۱:4.5,کتاب ۲:3.2,کتاب ۳:4.8,کتاب "
                              "۴:2.5,کتاب ۵:3.9,کتاب ۶:4.1,کتاب ۷:2.9,کتاب ۸:5.0");
    });
    // =============== پایان کدهای تست ===============
}

// ==========================================
// پردازش پیام‌های سرور
// ==========================================
void PublisherStatistics::processServerResponse(const QString &response)
{
    QStringList parts = response.split("||");
    if (parts.isEmpty())
        return;

    QString cmd = parts[0];

    if (cmd == "PUB_GENERAL_RESULT" && parts.size() >= 4) {
        ui->label_booknumber->setText(parts[1]);
        ui->label_avgscore->setText(parts[2]);
        ui->label_totalincome->setText(parts[3] + " تومان");
    } else if (cmd == "PUB_TOP_IDS_RESULT") {
        ui->tableWidget_maximum->setRowCount(0);
        if (parts.size() >= 2 && !parts[1].isEmpty() && parts[1] != "EMPTY") {
            QStringList ids = parts[1].split(",", Qt::SkipEmptyParts);
            for (const QString &id : ids) {
                requestTopBookInfo(id);
            }
        }
    } else if (cmd == "PUB_BOOK_INFO_TOP_RESULT" && parts.size() >= 5) {
        appendToTable(ui->tableWidget_maximum, parts[2], parts[3], parts[4]);
    } else if (cmd == "PUB_LOWEST_IDS_RESULT") {
        ui->tableWidget_minimum->setRowCount(0);
        if (parts.size() >= 2 && !parts[1].isEmpty() && parts[1] != "EMPTY") {
            QStringList ids = parts[1].split(",", Qt::SkipEmptyParts);
            for (const QString &id : ids) {
                requestLowestBookInfo(id);
            }
        }
    } else if (cmd == "PUB_BOOK_INFO_LOWEST_RESULT" && parts.size() >= 5) {
        appendToTable(ui->tableWidget_minimum, parts[2], parts[3], parts[4]);
    } else if (cmd == "PUB_SALES_CHART_RESULT") {
        drawPieChart(parts);
    } else if (cmd == "PUB_SCORE_CHART_RESULT") {
        drawBarChart(parts);
    }
}

// ==========================================
// توابع کمکی رسم UI
// ==========================================
void PublisherStatistics::appendToTable(QTableWidget *table,
                                        const QString &name,
                                        const QString &sales,
                                        const QString &score)
{
    int row = table->rowCount();
    table->insertRow(row);

    QTableWidgetItem *nameItem = new QTableWidgetItem(name);
    nameItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *salesItem = new QTableWidgetItem(sales);
    salesItem->setTextAlignment(Qt::AlignCenter);

    QTableWidgetItem *scoreItem = new QTableWidgetItem(score);
    scoreItem->setTextAlignment(Qt::AlignCenter);

    table->setItem(row, 0, nameItem);
    table->setItem(row, 1, salesItem);
    table->setItem(row, 2, scoreItem);
}

void PublisherStatistics::drawPieChart(const QStringList &dataParts)
{
    if (dataParts.size() < 2)
        return;
    QStringList slices = dataParts[1].split(",", Qt::SkipEmptyParts);

    QPieSeries *series = new QPieSeries();

    for (const QString &sliceData : slices) {
        QStringList info = sliceData.split(":");
        if (info.size() == 2) {
            series->append(info[0], info[1].toDouble());
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("سهم هر کتاب از کل فروش");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignRight);

    ui->tab_sahmbookbarSales->setChart(chart);
    ui->tab_sahmbookbarSales->setRenderHint(QPainter::Antialiasing);
}

void PublisherStatistics::drawBarChart(const QStringList &dataParts)
{
    if (dataParts.size() < 2)
        return;
    QStringList bars = dataParts[1].split(",", Qt::SkipEmptyParts);

    QBarSeries *series = new QBarSeries();
    QBarSet *scoreSet = new QBarSet("میانگین امتیاز");
    QStringList categories;

    for (const QString &barData : bars) {
        QStringList info = barData.split(":");
        if (info.size() == 2) {
            categories << info[0];
            *scoreSet << info[1].toDouble();
        }
    }

    series->append(scoreSet);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("میانگین امتیاز هر کتاب");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 5);
    axisY->setLabelFormat("%.1f");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->tab_avgscore->setChart(chart);
    ui->tab_avgscore->setRenderHint(QPainter::Antialiasing);

    int barCount = categories.size();
    ui->tab_avgscore->setMinimumWidth(qMax(600, barCount * 80));
}
void PublisherStatistics::refreshStatistics()
{
    requestGeneralStats();
    requestTopBooksIds();
    requestLowestBooksIds();
    requestSalesChartData();
    requestScoreChartData();
}
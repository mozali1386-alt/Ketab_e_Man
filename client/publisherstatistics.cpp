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

void PublisherStatistics::requestGeneralStats()
{
    QString message = "GET_PUB_GENERAL_STATS";
    m_client->sendMessage(message);
}

void PublisherStatistics::requestTopBooksIds()
{
    QString message = "GET_PUB_TOP_BOOKS_IDS";
    m_client->sendMessage(message);
}

void PublisherStatistics::requestTopBookInfo(const QString &bookId)
{
    QString message = "GET_PUB_BOOK_INFO_TOP||" + bookId;
    m_client->sendMessage(message);
}

void PublisherStatistics::requestLowestBooksIds()
{
    QString message = "GET_PUB_LOWEST_BOOKS_IDS";
    m_client->sendMessage(message);
}

void PublisherStatistics::requestLowestBookInfo(const QString &bookId)
{
    QString message = "GET_PUB_BOOK_INFO_LOWEST||" + bookId;
    m_client->sendMessage(message);
}

void PublisherStatistics::requestSalesChartData()
{
    QString message = "GET_PUB_SALES_CHART";
    m_client->sendMessage(message);
}

void PublisherStatistics::requestScoreChartData()
{
    QString message = "GET_PUB_SCORE_CHART";
    m_client->sendMessage(message);
}

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
    } else if (cmd == "PUB_SCORE_CHART_RESULT") {
        drawBarChart(parts);
    } else if (cmd == "PUB_DAILY_SALES_RESULT") { // <--- اضافه شد
        drawDailySalesChart(parts);
    }
}

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
    requestDailySalesChartData();
}
void PublisherStatistics::setClient(ClientSocketManager *client)
{
    m_client = client;
    connect(m_client,
            &ClientSocketManager::messageReceived,
            this,
            &PublisherStatistics::processServerResponse);
}
void PublisherStatistics::requestDailySalesChartData()
{
    QString message = "GET_PUB_DAILY_SALES_CHART";
    //m_client->sendMessage(message);

    // شبیه‌سازی جواب سرور
    QTimer::singleShot(400, this, [=]() {
        // داده‌های فرضی برای ۷ روز
        processServerResponse("PUB_DAILY_SALES_RESULT||روز ۱:12,روز ۲:18,روز ۳:14,روز ۴:25,روز "
                              "۵:20,روز ۶:30,روز ۷:28");
    });
}
void PublisherStatistics::drawDailySalesChart(const QStringList &dataParts)
{
    if (dataParts.size() < 2)
        return;
    QStringList points = dataParts[1].split(",", Qt::SkipEmptyParts);
    QLineSeries *series = new QLineSeries();
    series->setName("تعداد فروش");
    QStringList categories;

    int maxSales = 0;
    for (int i = 0; i < points.size(); ++i) {
        QStringList info = points[i].split(":");
        if (info.size() == 2) {
            categories << info[0];
            int sales = info[1].toInt();
            series->append(i, sales);

            if (sales > maxSales)
                maxSales = sales;
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("نمودار فروش روزانه (۷ روز اخیر)");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxSales + 5);
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    ui->tab_dailysales->setChart(chart);
    ui->tab_dailysales->setRenderHint(QPainter::Antialiasing);
}
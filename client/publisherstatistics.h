#ifndef PUBLISHERSTATISTICS_H
#define PUBLISHERSTATISTICS_H

#include <QTableWidget>
#include <QWidget>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include "clientsocketmanager.h"

namespace Ui {
class PublisherStatistics;
}

class PublisherStatistics : public QWidget
{
    Q_OBJECT

public:
    explicit PublisherStatistics(QWidget *parent = nullptr);
    ~PublisherStatistics();
    void refreshStatistics();
    void setClient(ClientSocketManager *client);

private slots:
    void processServerResponse(const QString &response);

private:
    Ui::PublisherStatistics *ui;
    ClientSocketManager *m_client = nullptr;

    void requestGeneralStats();
    void requestTopBooksIds();
    void requestLowestBooksIds();
    void requestSalesChartData();
    void requestScoreChartData();

    void requestTopBookInfo(const QString &bookId);
    void requestLowestBookInfo(const QString &bookId);

    void drawPieChart(const QStringList &dataParts);
    void drawBarChart(const QStringList &dataParts);
    void appendToTable(QTableWidget *table,
                       const QString &name,
                       const QString &sales,
                       const QString &score);

    void requestDailySalesChartData();
    void drawDailySalesChart(const QStringList &dataParts);
};

#endif
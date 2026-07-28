//in the name of ALLAH
//YA MAHDI

#ifndef SERVERDASHBOARD_H
#define SERVERDASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QTimer>
#include "../ServerCore.h"
#include "ServerLogModel.h"

class ServerDashboard : public QWidget {
    Q_OBJECT

public:
    explicit ServerDashboard(ServerCore *serverCorePointer, QWidget *parent = nullptr);

    virtual ~ServerDashboard();

private slots:
    void onLogMessageGenerated(QString message);

    void onConnectedClientCountChanged(int count);

    void onStatsTimerTimeout();

    void onStartStopButtonClicked();

private:
    ServerCore *core;
    bool serverIsRunning;
    quint16 listenPort;

    QLabel *statusLabel;
    QLabel *onlineCountLabel;
    QLabel *totalRequestsLabel;
    QLabel *totalUsersLabel;
    QLabel *totalBooksLabel;
    QListView *logListView;
    QPushButton *startStopButton;

    ServerLogModel *logModel;
    QTimer *statsTimer;

    void setupUi();

    void refreshStatsLabels();
};

#endif

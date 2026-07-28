//in the name of ALLAH
//YA MAHDI

#include "ServerDashboard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFont>

ServerDashboard::ServerDashboard(ServerCore *serverCorePointer, QWidget *parent) : QWidget(parent) {
    core = serverCorePointer;
    serverIsRunning = false;
    listenPort = 5050;

    logModel = new ServerLogModel(this);

    setupUi();

    connect(core, &ServerCore::logMessageGenerated, this, &ServerDashboard::onLogMessageGenerated);
    connect(core, &ServerCore::connectedClientCountChanged, this, &ServerDashboard::onConnectedClientCountChanged);

    statsTimer = new QTimer(this);
    connect(statsTimer, &QTimer::timeout, this, &ServerDashboard::onStatsTimerTimeout);
    statsTimer->start(2000);

    refreshStatsLabels();
}

ServerDashboard::~ServerDashboard() {
}

void ServerDashboard::setupUi() {
    setWindowTitle("Ketab-e-Man Server Dashboard");
    resize(640, 480);

    QFont titleFont;
    titleFont.setPointSize(14);
    titleFont.setBold(true);

    QLabel *titleLabel = new QLabel("Ketab-e-Man Server");
    titleLabel->setFont(titleFont);

    startStopButton = new QPushButton("Start Server");
    connect(startStopButton, &QPushButton::clicked, this, &ServerDashboard::onStartStopButtonClicked);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(startStopButton);

    statusLabel = new QLabel("Status: Stopped");
    onlineCountLabel = new QLabel("Online Clients: 0");
    totalRequestsLabel = new QLabel("Total Requests: 0");
    totalUsersLabel = new QLabel("Total Users: 0");
    totalBooksLabel = new QLabel("Total Books: 0");

    QGroupBox *statsBox = new QGroupBox("Live Statistics");
    QVBoxLayout *statsLayout = new QVBoxLayout();
    statsLayout->addWidget(statusLabel);
    statsLayout->addWidget(onlineCountLabel);
    statsLayout->addWidget(totalRequestsLabel);
    statsLayout->addWidget(totalUsersLabel);
    statsLayout->addWidget(totalBooksLabel);
    statsBox->setLayout(statsLayout);

    logListView = new QListView();
    logListView->setModel(logModel);

    QGroupBox *logBox = new QGroupBox("Server Log");
    QVBoxLayout *logLayout = new QVBoxLayout();
    logLayout->addWidget(logListView);
    logBox->setLayout(logLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(statsBox);
    mainLayout->addWidget(logBox);

    setLayout(mainLayout);
}

void ServerDashboard::onLogMessageGenerated(QString message) {
    logModel->addLogMessage(message);
}

void ServerDashboard::onConnectedClientCountChanged(int count) {
    onlineCountLabel->setText("Online Clients: " + QString::number(count));
}

void ServerDashboard::onStatsTimerTimeout() {
    refreshStatsLabels();
}

void ServerDashboard::refreshStatsLabels() {
    totalRequestsLabel->setText("Total Requests: " + QString::number(core->getTotalRequestCount()));
    totalUsersLabel->setText("Total Users: " + QString::number(core->getDataStore().getUsersMap().size()));
    totalBooksLabel->setText("Total Books: " + QString::number(core->getDataStore().getBooksMap().size()));
    onlineCountLabel->setText("Online Clients: " + QString::number(core->getConnectedClientCount()));
}

void ServerDashboard::onStartStopButtonClicked() {
    if (!serverIsRunning) {
        bool success = core->startServer(listenPort);
        if (success) {
            serverIsRunning = true;
            statusLabel->setText("Status: Running on port " + QString::number(listenPort));
            startStopButton->setText("Stop Server");
        } else {
            statusLabel->setText("Status: Failed to start");
        }
    } else {
        core->stopServer();
        serverIsRunning = false;
        statusLabel->setText("Status: Stopped");
        startStopButton->setText("Start Server");
    }
}

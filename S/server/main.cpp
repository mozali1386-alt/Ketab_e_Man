//in the name of ALLAH
//YA MAHDI

#include <QApplication>
#include "ServerCore.h"
#include "gui/ServerDashboard.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ServerCore core;
    ServerDashboard dashboard(&core);
    dashboard.show();

    return a.exec();
}

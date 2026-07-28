//in the name of ALLAH
//YA MAHDI

#include <QApplication>
#include "clientsocketmanager.h"
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ساختن شیء اصلی شبکه
    ClientSocketManager clientManager;

    clientManager.connectToServer("127.0.0.1", 5555);
    LoginWindow w(&clientManager);
    w.show();

    return a.exec();
}
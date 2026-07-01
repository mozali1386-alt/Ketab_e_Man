//in the name of ALLAH
//YA MAHDI

#include "loginwindow.h"

#include <QApplication>
#include "User/NormalUser/NormalUser.h"
int main(int argc, char *argv[])
{
    NormalUser("ali", "Aki", "lkviv", 2);
    return QApplication::exec();
}

#include"UserLogin.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    UserLogin* userlogin = new UserLogin;
    userlogin->show();
    return a.exec();
}
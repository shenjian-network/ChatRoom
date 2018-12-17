#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient w;
    w.ConnectToHost();
    w.show();

    return a.exec();
}

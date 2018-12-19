#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient w;
    w.show();

/*
    if(!w.ConnectToHost()){
        // TODO connect failed GUI
        return 1;
    }
*/

    // TODO login GUI
    w.loginGUI();



    return a.exec();
}

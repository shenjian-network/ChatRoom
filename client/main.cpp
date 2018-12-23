#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient w;


    if(!w.ConnectToHost("10.60.102.252", 25998)){
        w.errorGUI("网络连接异常");
        return 0;
    }

    // 显示登录GUI
    w.loginGUI();


    return a.exec();
}

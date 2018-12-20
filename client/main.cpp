#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient w;


    if(!w.ConnectToHost("100.66.41.79", 25998)){
        w.errorGUI("网络连接异常");
        return 0;
    }

    // 显示登录GUI
    w.loginGUI();

    /*
    //w.ConnectToHost("100.66.41.79", 25998);
    w.chatRoomGUI();
*/
    return a.exec();
}

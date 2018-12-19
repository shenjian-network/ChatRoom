#include "tcpclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient w;


    w.ConnectToHost("127.0.0.1", 12306);


    // 显示登录GUI
    w.chatRoomGUI();

    return a.exec();
}

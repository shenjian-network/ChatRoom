#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>

namespace Ui {
class TcpClient;
}

class TcpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    bool ConnectToHost();

    void loginGUI();

    // msg box,
    void errorGUI();

    void chatRoomGUI();

    void changePwdGUI();

    void configGUI();

    // two conditions... if it is myself, yor'are out; if it is other, modify GUI
    void offline();

    // only one condition
    void online();

    void cls();

    void showText();

    // read config
    void setConfig();

private slots:
    // Signal func to handle read event
    //
    void readyRead();

    // Signal func to handle disconnection with Server
    void disconnected();

    // login func
    // send a packet containing and wait
    // get_string write

private:





    QTcpSocket *socket;
    Ui::TcpClient *ui;
};

#endif // TCPCLIENT_H

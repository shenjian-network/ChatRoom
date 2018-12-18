#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QListWidget>

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

    void insertListWidget(QString desc);

    void selectAll();

    void selectNone();

    void getCheckState();

private slots:
    // Signal func to handle read event
    //
    void readyRead();

    // Signal func to handle disconnection with Server
    void disconnected();

    // login func
    // send a packet containing and wait
    // get_string write

    void on_loginBtn_clicked();

    void on_signupBtn_clicked();

    void on_sendBtn_clicked();

private:
    QTcpSocket *socket;
    Ui::TcpClient *ui;
    QWidget *loginWindow;
    QWidget *chatRoomWindow;
    QListWidget * userList;
};

#endif // TCPCLIENT_H

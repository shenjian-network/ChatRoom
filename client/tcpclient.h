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

    void ConnectToHost();

    // TODO: it has an argument
    void Write();

private slots:
    // Signal func to handle read event
    void readyRead();

    // Signal func to handle disconnection with Server
    void disconnected();

private:
    QTcpSocket *socket;
    Ui::TcpClient *ui;
};

#endif // TCPCLIENT_H

#include "tcpclient.h"
#include "ui_tcpclient.h"

TcpClient::TcpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);
}

TcpClient::~TcpClient()
{
    delete ui;
}


bool TcpClient::ConnectToHost(){
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    qDebug() << "connecting...";
    socket->connectToHost("192.168.1.1", 12306);

    if(!socket->waitForConnected(5000)){
        qDebug() << "Error: " << socket->errorString();
        return false;
    }

    return true;
}


void TcpClient::readyRead(){
    qDebug() << "reading...";

    char packet_head[8 * 8]; // 8B
    socket->read(packet_head, 8*8);

    // TODO state machine
}

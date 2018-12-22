#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QListWidget>
#include <QTime>
#include <map>
#include "common/client_to_server.h"
#include "common/packet_head.h"
#include "common/server_to_client.h"

namespace Ui {
class TcpClient;
}

const unsigned int kPacketHeadLen = 8;
enum ReadState
{
    READ_PACKET_HEAD,
    READ_SERVER_TO_CLIENT_REPORT_SUCCESS,
    READ_SERVER_CLIENT_ONLINE,
    READ_SERVER_CLIENT_OFFLINE,
    READ_SERVER_TO_CLIENT_TEXT_SIMPLE_TEXT,
    READ_SERVER_TO_CLIENT_TEXT_FILE_INFO,
    READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN,
    READ_SERVER_TO_CLIENT_USER_SET_UPDATE
};

class TcpClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    bool ConnectToHost(const QString& ip, unsigned short port);

    void loginGUI();

    // msg box,
    void errorGUI(const unsigned short & err_type);

    void errorGUI(const QString& err);

    //显示更改密码成功窗口
    void changePwdSuccessGUI();

    void reportSuccess();

    void chatRoomGUI();

    void configGUI();

    // two conditions... if it is myself, yor'are out; if it is other, modify GUI
    void offline();

    // only one condition
    void online();

    void cls();

    void showText();

    //显示文件信息
    void showFileInfo();

    //将文件内容从包写入文件，注意在调用该函数前，文件内容已经被写入了ServerToClientTextFileContain包，因此这部分只要显示下载文件完成等即可
    void writeFileContain();

    // read config
    void setConfig();

    void insertListWidget(QString name, bool isOnline);

    void selectAll();

    void selectNone();

    void getCheckState(QVector<bool>& vecIsChecked,  QVector<QString>& vecName);

    void setUserStatus(QString name, bool isOnline);

    void showTextImpl(QString name, QString msg, QDateTime tm);

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

    // 在登录界面，点击“修改密码”，将显示对话框
    void on_changePwdBtn_clicked();

    // 在修改密码界面，点击“确认”，将发送
    void on_changePwdAckBtn_clicked();

    // 在修改密码界面，点击“取消”，将关闭窗口
    void on_changePwdCancelBtn_clicked();

    // 登录界面，显示密码
    void on_showPwdCheckBox_stateChanged();

    // 更新时间
    void timeUpdate();

private:
    PacketHead my_packet_head;
    ServerToClientReportSuccess my_server_to_client_report_success;
    ServerToClientInform my_server_to_client_inform;
    ServerToClientTextSimpleText my_server_to_client_simple_text;
    ServerToClientTextFileInfo my_server_to_client_file_info;
    ServerToClientTextFileContain my_server_to_client_text_file_contain;
    ServerToClientUserSetUpdate my_server_to_client_user_set_update;

    QTcpSocket *socket;
    Ui::TcpClient *ui;

private:
    QWidget *loginWindow;
    QWidget *chatRoomWindow;
    QWidget *changePwdWindow;
    QListWidget * userList;
    QTime time;

    QString username;
    QString password;
    QString ip;
    std::map<std::string, std::string> configData;
    unsigned short port;
    
};

#endif // TCPCLIENT_H

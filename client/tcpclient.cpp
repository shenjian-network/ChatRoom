#include "tcpclient.h"
#include "ui_tcpclient.h"

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSize>
#include <QMessageBox>
#include <QPainter>
#include <QDir>
#include <QTextBrowser>
#include <QCheckBox>
#include <QRegExpValidator>
#include <QDateTime>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QPlainTextEdit>
#include <QFileDialog>


TcpClient::TcpClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    loginWindow = nullptr;
    chatRoomWindow = nullptr;
    changePwdWindow = nullptr;
    userList = nullptr;
    preChatter = nullptr;

    curChatter = new QLabel;

    curChatter->setAlignment(Qt::AlignCenter);
    curChatter->setFixedHeight(50);
    curChatter->setStyleSheet("background: white; color: black;border-bottom: 1px solid rgb(230, 230, 255);");

    QFont ft;
    ft.setPointSize(15);
    curChatter->setFont(ft);

    // 右栏，包括消息窗口和发送消息栏
    rightStackLayout = new QStackedLayout;
    rightStackLayout->setStackingMode(QStackedLayout::StackOne);

    QTimer *timer=new QTimer(this);
    timer->start(1000); // 每次发射timeout信号时间间隔为1秒
    connect(timer,SIGNAL(timeout()),this,SLOT(timeUpdate()), Qt::QueuedConnection);
    time.start();
}

TcpClient::~TcpClient()
{
    delete ui;
    delete socket;
}


// 连接到服务器 (FINISHED)
bool TcpClient::ConnectToHost(const QString& ip, unsigned short port){
    socket = new QTcpSocket(this);
    this->ip = ip;
    this->port = port;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::QueuedConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()), Qt::QueuedConnection);

    qDebug() << "connecting...";
    socket->connectToHost(ip, port);

    if(!socket->waitForConnected(5000)){
        qDebug() << "Error: " << socket->errorString();
        return false;
    }

    qDebug() << "connect success";
    return true;
}



// 显示登录界面 (FINISHED)
void TcpClient::loginGUI(){
    // login window
    loginWindow = new QWidget;
    loginWindow->setWindowTitle("登录");
    loginWindow->setMaximumSize(500, 300);
    loginWindow->setMinimumSize(500, 300);

    QHBoxLayout * layout1 = new QHBoxLayout;
    QLabel *unameLabel = new QLabel("登录");
    QLineEdit *unameEdit = new QLineEdit();
    unameLabel->setBuddy(unameEdit);
    QRegExp rx("[^\u4e00-\u9fa5]+");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    unameEdit->setValidator(pReg);
    layout1->addWidget(unameLabel);
    layout1->addWidget(unameEdit);
    layout1->setContentsMargins(30, 10, 30, 10);

    QHBoxLayout * layout2 = new QHBoxLayout;
    QLabel *upwdLabel = new QLabel("密码");
    QLineEdit *upwdEdit = new QLineEdit();
    upwdEdit->setEchoMode(QLineEdit::Password);
    upwdEdit->setValidator(pReg);
    unameLabel->setBuddy(upwdEdit);
    layout2->addWidget(upwdLabel);
    layout2->addWidget(upwdEdit);
    layout2->setContentsMargins(30, 10, 30, 10);

    QHBoxLayout * layout3 = new QHBoxLayout;
    QCheckBox * rememberPwd = new QCheckBox;
    rememberPwd->setText("记住密码");   // 还没这个功能
    rememberPwd->setCheckable(true);
    QCheckBox * showPwd = new QCheckBox;
    showPwd->setText("显示密码");
    showPwd->setCheckable(true);
    layout3->addWidget(rememberPwd);
    layout3->addWidget(showPwd);
    layout3->setContentsMargins(100, 10, 100, 10);

    QHBoxLayout * layout4 = new QHBoxLayout;
    QPushButton * login = new QPushButton("登录");
    QPushButton * signup = new QPushButton("注册");
    QPushButton * changePwd = new QPushButton("修改密码");

    login->setStyleSheet("QPushButton{background-color:rgb(0, 150, 255)}"
                         "QPushButton:hover{background-color:rgb(0, 255, 255)}");
    signup->setStyleSheet("QPushButton{background-color:rgb(0, 150, 255)}"
                         "QPushButton:hover{background-color:rgb(0, 255, 255)}");
    changePwd->setStyleSheet("QPushButton{background-color:rgb(0, 150, 255)}"
                         "QPushButton:hover{background-color:rgb(0, 255, 255)}");

    layout4->addWidget(login);
    layout4->addWidget(signup);
    layout4->addWidget(changePwd);
    layout4->setContentsMargins(100, 10, 100, 10);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout3);
    mainLayout->addLayout(layout4);
    mainLayout->setMargin(50);

    connect(login, SIGNAL(clicked()), this, SLOT(on_loginBtn_clicked()), Qt::QueuedConnection);
    connect(signup, SIGNAL(clicked()), this, SLOT(on_signupBtn_clicked()), Qt::QueuedConnection);
    connect(changePwd, SIGNAL(clicked()), this, SLOT(on_changePwdBtn_clicked()), Qt::QueuedConnection);
    connect(showPwd, SIGNAL(stateChanged(int)), this, SLOT(on_showPwdCheckBox_stateChanged()), Qt::QueuedConnection);

    loginWindow->setLayout(mainLayout);
    loginWindow->resize(500, 300);
     loginWindow->setAutoFillBackground(true); // 这句要加上, 否则可能显示不出背景图.
     QPalette palette;
     qDebug() << QDir::currentPath()+ "/image/background.jpg";
     QPixmap pix;
     if(pix.load(QDir::currentPath()+ "/image/background.jpg")){
         palette.setBrush(QPalette::Window,
                 QBrush(pix.scaled(// 缩放背景图.
                     loginWindow->size(),
                     Qt::IgnoreAspectRatio,
                     Qt::SmoothTransformation)));             // 使用平滑的缩放方式
         loginWindow->setPalette(palette);                           // 给widget加上背景图
     }

    loginWindow->show();
}



// 显示错误信息窗口 (FINISHED)
void TcpClient::errorGUI(const unsigned short & err_type){
    QMessageBox *errorBox = new QMessageBox();

    switch (err_type) {
    case PacketHead::kS2CReportWrongPwd:
        errorBox->setText("登录失败，密码错误");
        break;
    case PacketHead::kS2CReportNoExist:
        errorBox->setText("登录失败，用户名不存在");
        break;
    case PacketHead::kS2CReportMustUpdate:
        errorBox->setText("登录失败，需要强制改密");
        break;
    case PacketHead::kS2CReportDuplicated:
        errorBox->setText("注册失败，用户名已存在");
        break;
    case PacketHead::kS2CReportNameNotAccess:
        errorBox->setText("注册失败，用户名不符合要求");
        break;
    case PacketHead::kS2CReportPwdNotAccess:
        errorBox->setText("注册失败，密码不符合要求");
        break;
    case PacketHead::kS2CReportLastPwdWrong:
        errorBox->setText("更改密码失败，原密码错误");
        break;
    case PacketHead::kS2CReportNowPwdNotAccess:
        errorBox->setText("更改密码失败，现密码不符合规范");
        break;
    }

    errorBox->show();
}

// 显示错误信息窗口，重载 （FINISHED）
void TcpClient::errorGUI(const QString& err){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("错误"), err);
}


// 显示聊天室的新窗口，用于聊天(TODO)
void TcpClient::chatRoomGUI(){
    chatRoomWindow = new QWidget;
    chatRoomWindow->setWindowTitle("ChatRoom");
    chatRoomWindow->setFixedSize(800, 630);

    QPalette pal(chatRoomWindow->palette());
    pal.setColor(QPalette::Background, QColor(46, 50, 56)); //设置背景黑色
    chatRoomWindow->setPalette(pal);

    // 左栏，包括登录信息、用户列表
    QVBoxLayout * leftLayout = new QVBoxLayout;

    QPushButton * config = new QPushButton("设置");
    config->setStyleSheet("QPushButton{background: rgb(46, 50, 56); color: white;}");
    leftLayout->addWidget(config);

    connect(config, SIGNAL(clicked()), this, SLOT(configGUI()), Qt::QueuedConnection);

    QHBoxLayout * subsublayout1 = new QHBoxLayout;
    QLabel * curUserLabel = new QLabel("     当前用户");
    QLineEdit * curUsername = new QLineEdit(this->username);
    curUsername->setEnabled(false);
    curUserLabel->setStyleSheet("QLabel{color: white}");
    curUsername->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout1->addWidget(curUserLabel);
    subsublayout1->addWidget(curUsername);


    QHBoxLayout * subsublayout2 = new QHBoxLayout;
    QLabel * ipLabel = new QLabel("          IP地址");
    QLineEdit * ipaddr = new QLineEdit(this->ip);
    ipaddr->setEnabled(false);
    ipLabel->setStyleSheet("QLabel{color: white}");
    ipaddr->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout2->addWidget(ipLabel);
    subsublayout2->addWidget(ipaddr);

    QHBoxLayout * subsublayout3 = new QHBoxLayout;
    QLabel * portLabel = new QLabel("          端口号");
    QLineEdit * port = new QLineEdit(QString::number(this->port, 10));
    port->setEnabled(false);
    portLabel->setStyleSheet("QLabel{color: white}");
    port->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout3->addWidget(portLabel);
    subsublayout3->addWidget(port);

    QHBoxLayout * subsublayout4 = new QHBoxLayout;
    QLabel * loginTimeLabel = new QLabel("     登入时间");
    QLineEdit * loginTime = new QLineEdit(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    loginTime->setEnabled(false);
    loginTimeLabel->setStyleSheet("QLabel{color: white}");
    loginTime->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    subsublayout4->addWidget(loginTimeLabel);
    subsublayout4->addWidget(loginTime);

    QHBoxLayout * subsublayout5 = new QHBoxLayout;
    QLabel * onlineTimeLabel = new QLabel("     在线时间");
    QLineEdit * onlineTime = new QLineEdit();
    QLabel * unit = new QLabel("秒");
    onlineTime->setEnabled(false);
    onlineTimeLabel->setStyleSheet("QLabel{color: white}");
    onlineTime->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white; }");
    unit->setStyleSheet("QLabel{color: white}");
    subsublayout5->addWidget(onlineTimeLabel);
    subsublayout5->addWidget(onlineTime);
    subsublayout5->addWidget(unit);

    QHBoxLayout * subsublayout6 = new QHBoxLayout;
    QLabel * statusLabel = new QLabel("     当前状态");
    QLineEdit * status = new QLineEdit("我在线上");
    status->setEnabled(false);
    statusLabel->setStyleSheet("QLabel{color: white}");
    status->setStyleSheet("QLineEdit{background: rgb(38, 41, 46); color: white;}");
    subsublayout6->addWidget(statusLabel);
    subsublayout6->addWidget(status);

    userList = new QListWidget;
    userList->setStyleSheet("QListWidget{background: rgb(46, 50, 56); border: 0px; "
                            "border-top: 1px solid rgb(0, 0, 0);}"
                            "QScrollBar:vertical{background: rgb(46, 50, 56);"
                            "width: 8px;}"
                            "QScrollBar::handle:vertical{background: rgb(58,63,69);"
                            "border: none;"
                            "min-height: 20px;}"
                            "QScrollBar::handle:vertical:hover{background: rgb(128,128,128);"
                            "border: none;"
                            "min-height: 20px;}"
                            "QScrollBar::add-line:vertical{border: none;}"
                            "QScrollBar::sub-line:vertical{border: none;}");
    userList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    leftLayout->addLayout(subsublayout1);
    leftLayout->addLayout(subsublayout2);
    leftLayout->addLayout(subsublayout3);
    leftLayout->addLayout(subsublayout4);
    leftLayout->addLayout(subsublayout5);
    leftLayout->addLayout(subsublayout6);
    leftLayout->addWidget(userList);


    rightStackLayout->addWidget(new QWidget);

    chatRoomMainLayout = new QHBoxLayout;
    chatRoomMainLayout->addLayout(leftLayout);
    chatRoomMainLayout->addLayout(rightStackLayout);
    chatRoomMainLayout->setStretchFactor(leftLayout, 1);
    chatRoomMainLayout->setStretchFactor(rightStackLayout, 2);
    chatRoomMainLayout->setMargin(0);

    chatRoomWindow->setLayout(chatRoomMainLayout);
    chatRoomWindow->show();
}

// *****辅助函数***** //

std::string QStringToString(const QString & myQstring)
{
    return std::string(myQstring.toLocal8Bit().constData());
}

std::string stringPadding(std::string myString, const unsigned int & len)
{
    myString.resize(len, 0);
    return myString;
}

//-----------------------//

//点击回看 发送回看包
void TcpClient::askForReview(){
    //当前框的对象，也就是要回看的人，如果所有人，则为空
    std::string toUser;

    //TODO
    /*获得toUser*/
    toUser = QStringToString(curChatter->text());
    if(toUser == "群聊"){
        toUser = "";
    }

    //发送配置文件包
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SText);
    sendPacketHead.set_function_type(PacketHead::kC2STextAskForTexts);

    sendPacketHead.set_length(36);

    //没有reviewLineCnt这一项
    if(configMap.find("reviewLineCnt") == configMap.end())
    {
        qDebug() << "reviewLineCnt Not Exists" << endl;
        return;
    }

    ClientToServerTextAskForTexts sendClientToServerTextAskForTexts(sendPacketHead, 
        atoi(configMap["reviewLineCnt"].c_str()), stringPadding(toUser, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerTextAskForTexts.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}

// 配置界面 （TODO）
//下面有个确定键，点击后，会更改GUI界面，并触发槽函数sendConfig
void TcpClient::configGUI(){
    configWindow = new QWidget;
    configWindow->setFixedSize(500, 300);

    QVBoxLayout * layout = new QVBoxLayout;

    QHBoxLayout * sublayout1 = new QHBoxLayout;
    QLabel * label1 = new QLabel("回看数");
    QLineEdit * value1 = new QLineEdit;
    value1->setValidator(new QIntValidator(100, 200, this));
    sublayout1->addWidget(label1);
    sublayout1->addWidget(value1);

    QHBoxLayout * sublayout2 = new QHBoxLayout;
    QLabel * label2 = new QLabel("字体大小");
    QLineEdit * value2 = new QLineEdit;
    value2->setValidator(new QIntValidator(4, 24, this));
    sublayout2->addWidget(label2);
    sublayout2->addWidget(value2);


    QHBoxLayout * sublayout3 = new QHBoxLayout;
    QLabel * label3 = new QLabel("字体颜色");
    QLineEdit * value3 = new QLineEdit;
    value3->setValidator(new QIntValidator(0, 7, this));
    sublayout3->addWidget(label3);
    sublayout3->addWidget(value3);


    QHBoxLayout * sublayout4 = new QHBoxLayout;
    QPushButton * ack = new QPushButton("确定");
    ack->setFixedSize(75, 30);
    sublayout4->addWidget(ack);

    layout->addLayout(sublayout1);
    layout->addLayout(sublayout2);
    layout->addLayout(sublayout3);
    layout->addLayout(sublayout4);
    configWindow->setLayout(layout);

    connect(ack, SIGNAL(clicked()), this, SLOT(sendConfig()), Qt::QueuedConnection);
    configWindow->show();
}


// 下线的操作 （FINISHED）
// 需要熄灭用户栏，如果下线的是自己，会退出
void TcpClient::offline(){
    QString name = my_server_to_client_inform.get_user_name();

    // 1. 下线的是自己，代表被T了
    if(name == this->username){
        chatRoomWindow->close();
        errorGUI("您被T了");
        exit(1);
    } else{
        setUserStatus(name, false);
    }
}

// 上线的操作（FINISHED）
// 需要点亮用户栏
void TcpClient::online(){
    QString name = my_server_to_client_inform.get_user_name();

    setUserStatus(name, true);
}

// 清屏，清除对话框的内容 （TODO）
void TcpClient::cls(){
    QWidget* cur = rightStackLayout->currentWidget();
    QTextBrowser* curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
    curTextBrowser->clear();

    askForReview();
}

// 显示文本 （TODO）
void TcpClient::showText(){
    char* text = my_server_to_client_simple_text.get_simple_text_contain();
    char* from = my_server_to_client_simple_text.get_user_from_name();
    char* to = my_server_to_client_simple_text.get_user_to_name();
    char* time = my_server_to_client_simple_text.get_now_time();

    qDebug() << "text:" << text;
    qDebug() << "from:" << from;
    qDebug() << "time:" << time;

    if(to[0] == '\0'){
        QWidget * cur = rightStackLayout->itemAt(user2Index["群聊"])->widget();
        QTextBrowser* curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
        curTextBrowser->append(QString(from) + " <" + QString(time) + "> ");
        curTextBrowser->append(QString(text));
    }else{
        if(QString(from) == username){
            showTextImpl(QString(to), QString(text), QString(time), true);
        }else{
            showTextImpl(QString(from), QString(text), QString(time));
        }
    }
}

std::string getKey(std::string senderNameString, std::string recvNameString, std::string fileNameSting)
{
    return stringPadding(senderNameString, 32) + stringPadding(recvNameString, 32) + stringPadding(fileNameSting, 64);
}

//TODO
void TcpClient::showFileTransferring(std::string senderName, std::string recvName, std::string fileName, bool isSender)
{
    //获得相应的fileToShow
    std::string fileKey = getKey(senderName, recvName, fileName);
    fileTrans fileToShow;
    if(isSender)
    {
        if(sendFile.find(fileKey) == sendFile.end())
            return;
        fileToShow = sendFile[fileKey];
    }
    else
    {
        if(recvFile.find(fileKey) == recvFile.end())
            return;
        fileToShow = recvFile[fileKey];
    }
    
    int perCent = 100 * FILEBUFFERSIZE * fileToShow.blockCnt / fileToShow.len;
    //TODO
    //GUI显示进度百分比perCent
    pdlg->setValue(perCent);
}

//TODO
void TcpClient::errorFileTransferring(std::string senderName, std::string recvName, std::string fileName)
{
    //显示已经有文件正在发送，不能在这时候发送
}

//TODO
void TcpClient::cancelFileTransferring(std::string senderName, std::string recvName, std::string fileName, bool isSender)
{
    //显示文件已经被取消传输
}

//TODO
void TcpClient::doneFileTransferring(std::string senderName, std::string recvName, std::string fileName, bool isSender)
{
    //显示文件传输完成
}


int getFileLen(const char * fileName)
{
    int fileLen;
    FILE *fp;
    fp = fopen(fileName,"rb");
    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    fclose(fp);
    return fileLen;
}

//TODO
void TcpClient::tryToSend(QString fileName)
{   
    /*
    点击发送文件的按钮触发的事件,首先需要获得接收方用户名recvName, 文件名fileName
    */
    QString recvName = curChatter->text();

    qDebug() << "要发送的文件" << fileName;
    qDebug() << "发送的用户" << recvName;

    pdlg = new QProgressDialog;


    std::string senderNameString = QStringToString(username);
    std::string recvNameString = QStringToString(recvName);
    std::string fileNameString = QStringToString(fileName);
    int fileLen = getFileLen(fileNameString.c_str());

    qDebug() << "文件长度" << fileLen;
    
    /*
    首先在sendFile这个Map中查看是否存在，如果存在则说明正在发送，弹窗显示不能继续发送,
    否则向对端发送一个请求发送包
    */
    std::string sendFileKey = getKey(senderNameString, recvNameString, fileNameString);
    
    if(sendFile.find(sendFileKey) == sendFile.end())
    {
        //Map中找不到，说明不是正在发送，可以发送，要显示发送的进度
        FILE* fd = fopen(fileNameString.c_str(), "rb");
        sendFile[sendFileKey] = fileTrans(fd, 0, fileLen);

        showFileTransferring(senderNameString, recvNameString, fileNameString, true);
        //给对端发请求发送包
        PacketHead sendPacketHead;

        sendPacketHead.set_packet_type(PacketHead::kC2CFileNotify);
        sendPacketHead.set_function_type(PacketHead::kC2CFileNotifyRequest);

        sendPacketHead.set_length(132);

        SenderToReceiverFileNotify sendSenderToReceiverFileNotify(sendPacketHead,
            stringPadding(senderNameString, 32).c_str(), stringPadding(recvNameString, 32).c_str(),
            stringPadding(fileNameString, 64).c_str(), fileLen);

        char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
        sendSenderToReceiverFileNotify.get_string(tmpStr);
        socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

        delete[] tmpStr;
    }
    else
    {
        //显示文件正在发送，不能发送
        errorFileTransferring(senderNameString, recvNameString, fileNameString);
    }
}

//TODO
void TcpClient::showTryToSend()
{
    //receiver收到请求发送包，要显示出相关的文件信息，并提供相关的按钮（接收/取消），如果点击接收，那么要有是否覆盖的提示

    // 发送者，文件名
    senderName = my_sender_to_receiver_file_notify.get_sender_name();
    fileName = my_sender_to_receiver_file_notify.get_file_name();
    fileLen = my_sender_to_receiver_file_notify.get_file_size();

    fileWindow = new QWidget;

    QVBoxLayout* mainLayout = new QVBoxLayout;

    QLabel* info = new QLabel(senderName + "将向你发送文件\"" + fileName + "\", 大小为" +  QString(fileLen)  +"字节，是否同意?");

    QHBoxLayout* sublayout = new QHBoxLayout;
    QPushButton * ack = new QPushButton("接受");
    QPushButton * rjt = new QPushButton("拒绝");
    sublayout->addWidget(ack);
    sublayout->addWidget(rjt);

    mainLayout->addWidget(info);
    mainLayout->addLayout(sublayout);
    fileWindow->setLayout(mainLayout);

    connect(ack, SIGNAL(clicked()), this, SLOT(acceptRecv()), Qt::QueuedConnection);
    connect(ack, SIGNAL(clicked()), this, SLOT(cancelRecvFileDataActive()), Qt::QueuedConnection);

    fileWindow->show();
}

//TODO
void TcpClient::acceptRecv()
{
    fileWindow->close();
    /*
    点击文件接收按钮触发的事件,首先要获得发送方用户名senderName, 文件名称fileName,文件大小fileLen
    */    

    /*
    open本地文件，打开方式为trunc
    然后在recvFile这个Map中增加一项，下标为发送方+接收方+文件名
    值为fileTrans,不需要len这一项
    */
    std::string senderNameString = QStringToString(senderName);
    std::string recvNameString = QStringToString(username);
    std::string fileNameString = QStringToString(fileName);
    FILE* fd = fopen(fileNameString.c_str(), "wb");

    std::string recvFileKey = getKey(senderNameString, recvNameString, fileNameString);


    recvFile[recvFileKey] = fileTrans(fd, 0, fileLen);
    
    /*
    向对端发送同意接收包
    */
    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2CFileNotify);
    sendPacketHead.set_function_type(PacketHead::kC2CFileNotifyAccept);

    sendPacketHead.set_length(132);

    SenderToReceiverFileNotify recvSenderToReceiverFileNotify(sendPacketHead,
        stringPadding(senderNameString, 32).c_str(), stringPadding(recvNameString, 32).c_str(),
        stringPadding(fileNameString, 64).c_str(), 0);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    recvSenderToReceiverFileNotify.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    qDebug() << "it sends";
}


void TcpClient::sendFileData()
{
    qDebug() << "it works";
    /*Sender收到请求发送包后，从中读取块，然后发送*/
    /*
    注意：由于common包没有处理尾0的情况，因此username只允许到31
    */
    std::string senderNameString = std::string(my_sender_to_receiver_file_notify.get_sender_name());
    std::string recvNameString = std::string(my_sender_to_receiver_file_notify.get_receiver_name());
    std::string fileNameString = std::string(my_sender_to_receiver_file_notify.get_file_name());
    unsigned int blockCnt = my_sender_to_receiver_file_notify.get_block_num();

    qDebug() << QString(senderNameString.c_str());
    qDebug() << QString(recvNameString.c_str());
    qDebug() << QString(fileNameString.c_str());
    qDebug() << blockCnt;



    std::string sendFileKey = getKey(senderNameString, recvNameString, fileNameString);

    if(sendFile.find(sendFileKey) == sendFile.end())
        return;

    qDebug() << "get here";
    auto myFileTrans = sendFile[sendFileKey];

    char* fileContain = new char [FILEBUFFERSIZE + 1];
    unsigned int readSize = fread(fileContain, 1, FILEBUFFERSIZE, myFileTrans.fd);

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2CFileData);
    sendPacketHead.set_length(kFileDataLen);

    if(readSize < FILEBUFFERSIZE)//读到了文件结尾
    {
        blockCnt = 0xFFFF;
        fclose(myFileTrans.fd);
        sendFile.erase(sendFileKey);
        doneFileTransferring(senderNameString, recvNameString, fileNameString, true);
    }
    else   
        showFileTransferring(senderNameString, recvNameString, fileNameString, true);

    SenderToReceiverFileData senderToReceiverFileData(sendPacketHead,
        stringPadding(senderNameString, 32).c_str(), stringPadding(recvNameString, 32).c_str(),
        stringPadding(fileNameString, 64).c_str(), blockCnt, fileContain);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    senderToReceiverFileData.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
    delete[] fileContain;
}

void TcpClient::writeDataAndRequest()
{
    /*rec从包中获得数据并向send发送新的请求*/
    std::string senderNameString = std::string(my_sender_to_receiver_file_data.get_sender_name());
    std::string recvNameString = std::string(my_sender_to_receiver_file_data.get_receiver_name());
    std::string fileNameString = std::string(my_sender_to_receiver_file_data.get_file_name());
    unsigned int blockCnt = my_sender_to_receiver_file_notify.get_block_num();
    
    std::string recvFileKey = getKey(senderNameString, recvNameString, fileNameString);

    if(recvFile.find(recvFileKey) == recvFile.end())
        return;

    auto myFileTrans = recvFile[recvFileKey];

    if(blockCnt != 0xFFFF)
    {
        fwrite(my_sender_to_receiver_file_data.get_file_contain(), 1, FILEBUFFERSIZE, myFileTrans.fd);

        showFileTransferring(senderNameString, recvNameString, fileNameString, false);
        /*
        向对端发送请求接收包
        */
        ++blockCnt;
        recvFile[recvFileKey].blockCnt = blockCnt;
        PacketHead sendPacketHead;

        sendPacketHead.set_packet_type(PacketHead::kC2CFileNotify);
        sendPacketHead.set_function_type(PacketHead::kC2CFileNotifyAccept);

        sendPacketHead.set_length(132);

        SenderToReceiverFileNotify recvSenderToReceiverFileNotify(sendPacketHead,
            stringPadding(senderNameString, 32).c_str(), stringPadding(recvNameString, 32).c_str(),
            stringPadding(fileNameString, 64).c_str(), blockCnt);

        char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
        recvSenderToReceiverFileNotify.get_string(tmpStr);
        socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

        delete[] tmpStr;
    }
    else
    {
        fwrite(my_sender_to_receiver_file_data.get_file_contain(), 1, 
            myFileTrans.len - myFileTrans.blockCnt * FILEBUFFERSIZE, myFileTrans.fd);
        
        fclose(myFileTrans.fd);
        recvFile.erase(recvFileKey);
        doneFileTransferring(senderNameString, recvNameString, fileNameString, false);
    }
}

//主动取消发送
void TcpClient::cancelSendFileDataActive()
{
    fileWindow->close();
    //需要知道recvName和fileName
    QString recvName;
    QString fileName;

    //TODO

    std::string senderNameString = QStringToString(username);
    std::string recvNameString = QStringToString(recvName);
    std::string fileNameString = QStringToString(fileName);

    cancelFileTransferring(senderNameString, recvNameString, fileNameString, true);//GUI显示取消发送

    std::string sendFileKey = getKey(senderNameString, recvNameString, fileNameString);
    
    if(sendFile.find(sendFileKey) == sendFile.end())
        return;
    
    /*向对面发送取消发送包*/

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2CFileNotify);
    sendPacketHead.set_function_type(PacketHead::kC2CFileNotifyCancelSend);  //注册
    sendPacketHead.set_length(132);
    int fileLen = sendFile[sendFileKey].len;

    SenderToReceiverFileNotify sendSenderToReceiverFileNotify(sendPacketHead,
            stringPadding(senderNameString, 32).c_str(), stringPadding(recvNameString, 32).c_str(),
            stringPadding(fileNameString, 64).c_str(), fileLen);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendSenderToReceiverFileNotify.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
    
    /*发送完成*/
    
    fclose(sendFile[sendFileKey].fd);
    sendFile.erase(sendFileKey);
}

//主动取消接收
void TcpClient::cancelRecvFileDataActive()
{
    //需要知道senderName和fileName


    std::string senderNameString = QStringToString(senderName);
    std::string recvNameString = QStringToString(username);
    std::string fileNameString = QStringToString(fileName);

    cancelFileTransferring(senderNameString, recvNameString, fileNameString, false);//GUI显示取消发送

    std::string recvFileKey = getKey(senderNameString, recvNameString, fileNameString);

    if(recvFile.find(recvFileKey) == recvFile.end())
        return;

    //TODO
    /*向对面发送取消接收包*/

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2CFileNotify);
    sendPacketHead.set_function_type(PacketHead::kC2CFileNotifyCancelRecv);  //注册
    sendPacketHead.set_length(132);
    int fileLen = recvFile[recvFileKey].len;

    SenderToReceiverFileNotify sendSenderToReceiverFileNotify(sendPacketHead,
            stringPadding(senderNameString, 32).c_str(), stringPadding(recvNameString, 32).c_str(),
            stringPadding(fileNameString, 64).c_str(), fileLen);

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendSenderToReceiverFileNotify.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    /*发送完成*/

    fclose(recvFile[recvFileKey].fd);
    remove(fileNameString.c_str());
    recvFile.erase(recvFileKey);
}

//被动取消发送
void TcpClient::cancelSendFileDataPassive()
{
    std::string senderNameString = std::string(my_sender_to_receiver_file_data.get_sender_name());
    std::string recvNameString = std::string(my_sender_to_receiver_file_data.get_receiver_name());
    std::string fileNameString = std::string(my_sender_to_receiver_file_data.get_file_name());

    cancelFileTransferring(senderNameString, recvNameString, fileNameString, true);//GUI显示取消发送

    std::string sendFileKey = getKey(senderNameString, recvNameString, fileNameString);
    
    if(sendFile.find(sendFileKey) == sendFile.end())
        return;
    
    fclose(sendFile[sendFileKey].fd);
    sendFile.erase(sendFileKey);
}


//被动取消接收
void TcpClient::cancelRecvFileDataPassive()
{
    std::string senderNameString = std::string(my_sender_to_receiver_file_data.get_sender_name());
    std::string recvNameString = std::string(my_sender_to_receiver_file_data.get_receiver_name());
    std::string fileNameString = std::string(my_sender_to_receiver_file_data.get_file_name());

    cancelFileTransferring(senderNameString, recvNameString, fileNameString, false);//GUI显示取消发送

    std::string recvFileKey = getKey(senderNameString, recvNameString, fileNameString);

    if(recvFile.find(recvFileKey) == recvFile.end())
        return;

    fclose(recvFile[recvFileKey].fd);
    remove(fileNameString.c_str());
    recvFile.erase(recvFileKey);
}


std::string TcpClient::singleConfigString(std::string configKey)
{
    return configKey + ' ' + configMap[configKey] + '\n';
}

//将config转换成string
std::string TcpClient::configString()
{
    std::string ret;
    ret += singleConfigString("reviewLineCnt");
    //TODO
    /*增加其他的配置项*/
    ret += singleConfigString("fontsize");

    ret += singleConfigString("fontcolor");

    return ret;
}

//发送配置更新包
void TcpClient::sendConfig()
{
    QLineEdit* line1 = static_cast<QLineEdit*>(configWindow->layout()->itemAt(0)->layout()->itemAt(1)->widget());
    qDebug() << line1->text();
    configMap["reviewLineCnt"] = QStringToString(line1->text());

    QLineEdit* line2 = static_cast<QLineEdit*>(configWindow->layout()->itemAt(1)->layout()->itemAt(1)->widget());
    qDebug() << line2->text();
    configMap["fontsize"] = QStringToString(line2->text());

    QLineEdit* line3 = static_cast<QLineEdit*>(configWindow->layout()->itemAt(2)->layout()->itemAt(1)->widget());
    qDebug() << line3->text();
    configMap["fontcolor"] = QStringToString(line3->text());

    // 真正的设置
    setConfigImpl(line2->text().toInt(), line3->text().toInt());

    configWindow->close();

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SUserSet);
    sendPacketHead.set_function_type(PacketHead::kC2SUserSetUpdate);

    std::string myConfigString = configString();
//    printf(myConfigString.c_str());
//    fflush(stdout);

    sendPacketHead.set_length(myConfigString.length());

    ClientToServerUserSetUpdate sendClientToServerUserSetUpdate(sendPacketHead, myConfigString.c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerUserSetUpdate.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}

//根据初始发送配置包的内容进行设置配置 （TODO）
void TcpClient::setConfig(){
    std::string configData = std::string(my_server_to_client_user_set_update.get_user_set_data());
    printf(configData.c_str());

    std::istringstream configDataStream(configData);
    std::string configKey, configValue;
    while(configDataStream >> configKey >> configValue)
        configMap[configKey] = configValue;

    for(auto item: configMap){
        printf("%s, %s\n", item.first.c_str(), item.second.c_str());
        fflush(stdout);
    }


    //TODO
    /*
    根据configMap中的键值对来进行相应的设置
    */
    setConfigImpl(atoi(configMap["fontsize"].c_str()), atoi(configMap[configKey].c_str()));

}

// 向用户列表中添加一项 （FINISHED)
void TcpClient::insertListWidget(QString name, bool isOnline){
    static int index = 0;
    QListWidgetItem * item = new QListWidgetItem;
    QLabel * userLabel = new QLabel;
    ClickableLabel * user = new ClickableLabel(name, userLabel);
    user->setAlignment(Qt::AlignVCenter);
    user->setFixedHeight(65);
    user->setMargin(23);
    item->setSizeHint(user->sizeHint());

    QFont ft;
    ft.setPointSize(12);
    user->setFont(ft);

    connect(user, SIGNAL(clicked()), this, SLOT(userLabelClicked()), Qt::QueuedConnection);

    if(isOnline){
        user->setEnabled(true);
        user->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                 "QLabel{color: rgb(255, 255, 255)}"
                                 "QLabel:hover{background: rgb(58,63,69)}");
    }else{
        user->setEnabled(false);
        user->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                            "QLabel:hover{background: rgb(58,63,69)}"
                            "QLabel{color: rgb(128, 128, 128)}");
    }

    InitRightLayout();

    user2Index.insert(name, ++index);

    userList->addItem(item);
    userList->setItemWidget(item, user);
}


// 选择用户列表中所有用户（FINISHED)
void TcpClient::selectAll(){
    int count = userList->count();
    QListWidgetItem * item;
    QWidget * widget;
    QCheckBox * box;
    for(int i = 0;i < count; ++i){
        item = userList->item(i);
        widget = userList->itemWidget(item);
        box = static_cast<QCheckBox*>(widget);
        box->setChecked(true);
    }
}

// 不选用户列表所有用户（FINISHED)
void TcpClient::selectNone(){
    int count = userList->count();
    QListWidgetItem * item;
    QWidget * widget;
    QCheckBox * box;
    for(int i = 0;i < count; ++i){
        item = userList->item(i);
        widget = userList->itemWidget(item);
        box = static_cast<QCheckBox*>(widget);
        box->setChecked(false);
    }
}


// 得到用户列表的用户选中的状态（FINISHED）
void TcpClient::getCheckState(QVector<bool>& vecIsChecked,  QVector<QString>& vecName){
    int count = userList->count();
    bool isChecked;
    QListWidgetItem * item;
    QWidget * widget;
    QCheckBox * box;
    QString name;

    vecIsChecked.clear();
    vecName.clear();
    for(int i = 0;i < count; ++i){
        item = userList->item(i);
        widget = userList->itemWidget(item);
        box = static_cast<QCheckBox*>(widget);
        isChecked = box->isChecked();
        vecIsChecked.push_back(isChecked);
        name = box->text();
        vecName.push_back(name);
    }
}

// 设置用户名的状态
void TcpClient::setUserStatus(QString name, bool isOnline){
    int count = userList->count();

    QListWidgetItem * item;
    QWidget * widget;
    QLabel * box;
    QString curName;

    qDebug() << "setUserStatus1";
    int flag = false;
    // O(n)遍历
    for(int i = 0;i < count; ++i){
        item = userList->item(i);
        widget = userList->itemWidget(item);
        box = static_cast<QLabel*>(widget);
        curName = box->text();

        if(curName == name){
            flag = true;
            if(isOnline){
                box->setEnabled(true);
                box->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                         "QLabel{color: rgb(255, 255, 255)}"
                                         "QLabel:hover{background: rgb(58,63,69)}");
            } else {
                box->setEnabled(false);
                box->setStyleSheet("QLabel{background: rgb(46, 50, 56)}"
                                    "QLabel:hover{background: rgb(58,63,69)}"
                                    "QLabel{color: rgb(128, 128, 128)}");
            }
            break;
        }
    }
    qDebug() << "setUserStatus2";

    // 如果没找到username, 报错
    if(!flag){
        qDebug() << "Fatal Error: 没有这个用户！" << name;
    }
}



// 更改密码成功GUI （TODO）
void TcpClient::changePwdSuccessGUI(){
    errorGUI("修改密码成功");
}



// 报道成功操作 (FINISHED)
// 1. 登录成功， 进入chatRoomGUI
// 2. 注册成功，再登录一次
void TcpClient::reportSuccess(){
    auto length = my_server_to_client_report_success.get_packet_head().get_length();

    // 2. 注册成功包
    if(length == 0){
        errorGUI("注册成功，请重新登录");
        QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
        QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
        QLineEdit* line = static_cast<QLineEdit*>(item->widget());
        line->clear(); // 清空密码就行了
    }
    else{ // 1. 登录成功包
        if(my_server_to_client_report_success.get_packet_head().get_function_type() == PacketHead::kS2CReportSuccessDup){
            errorGUI("您把别人T了");
        }

        qDebug() << "登录成功" << this->username;
        loginWindow->close();
        chatRoomGUI();

        int num = my_server_to_client_report_success.get_user_num();
        qDebug() << "所有用户" << num;
        char** uinfo = my_server_to_client_report_success.get_user_status();
        char name[32];
        for(int i = 0;i < num; ++i){
            memcpy(name, uinfo[i], 32);
            qDebug() << name << " " << int(uinfo[i][32]-'0');
            insertListWidget(QString(name), int(uinfo[i][32]-'0'));
        }

        insertListWidget(QString("群聊"), true);
    }
}


// 显示文本内容（TODO）
void TcpClient::showFileInfo(){

}

// 写入文件内容 （TODO）
void TcpClient::writeFileContain(){

}


// 显示消息，带GUI
void TcpClient::showTextImpl(QString name, QString msg, QString tm, bool isMyself){
    QWidget * cur = rightStackLayout->itemAt(user2Index[name])->widget();
    QTextBrowser* curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
    if(isMyself){
        name = username;
    }
    curTextBrowser->append(name + " <" + tm + "> ");
    curTextBrowser->append(msg);
}


void TcpClient::InitRightLayout(){
    QWidget * right = new QWidget;

    QVBoxLayout* rightLayout = new QVBoxLayout;
    // 右栏，包括消息窗口和发送消息栏

    QLabel* label = new QLabel;
    label->setAlignment(Qt::AlignCenter);
    label->setFixedHeight(50);
    label->setStyleSheet("background: white; color: black;border-bottom: 1px solid rgb(230, 230, 255);");

    QFont ft;
    ft.setPointSize(15);
    label->setFont(ft);

    rightLayout->addWidget(label);

    QTextBrowser * textfield = new QTextBrowser;
    textfield->setStyleSheet("border:0px; border-bottom: 1px solid rgb(230,230,250);");
    rightLayout->addWidget(textfield);

    QWidget * textEditArea = new QWidget;

    QPlainTextEdit * textedit = new QPlainTextEdit;
    textedit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    textedit->setStyleSheet("border:0px; color: black;");
    textedit->setParent(textEditArea);
    textedit->setGeometry(0, 0, 530, 200);
    ft.setPointSize(12);
    textedit->setFont(ft);

    QPushButton * send = new QPushButton("Send");
    send->setStyleSheet("QPushButton:hover{background: rgb(248, 248, 248);}");
    send->setFixedSize(95, 30);
    send->setGeometry(415, 130, 95, 30);
    send->setParent(textEditArea);
    send->setShortcut(Qt::Key_F1);

    connect(send, SIGNAL(clicked()),  this, SLOT(on_sendBtn_clicked()), Qt::QueuedConnection);

    QPushButton * review = new QPushButton("Review");
    review->setStyleSheet("QPushButton:hover{background: rgb(248, 248, 248);}");
    review->setFixedSize(95, 30);
    review->setGeometry(300, 130, 95, 30);
    review->setParent(textEditArea);

    connect(review, SIGNAL(clicked()),  this, SLOT(cls()), Qt::QueuedConnection);

    QPushButton * file = new QPushButton("File");
    file->setStyleSheet("QPushButton:hover{background: rgb(248, 248, 248);}");
    file->setFixedSize(95, 30);
    file->setGeometry(185, 130, 95, 30);
    file->setParent(textEditArea);

    connect(file, SIGNAL(clicked()),  this, SLOT(on_fileDialogBtn_clicked()), Qt::QueuedConnection);

    rightLayout->addWidget(textEditArea);
    rightLayout->setStretchFactor(textfield, 2);
    rightLayout->setStretchFactor(textEditArea, 1);
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);

    right->setLayout(rightLayout);
    // stack
    rightStackLayout->addWidget(right);
}


void TcpClient::setConfigImpl(int fontsize, int color){
    int num = rightStackLayout->count();
    QWidget* cur;
    QTextBrowser* curTextBrowser;
    for(int i = 1;i < num; ++i){
        cur = rightStackLayout->itemAt(i)->widget();
        qDebug() << cur->layout()->count();
        curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
        curTextBrowser->setStyleSheet("font-size: " + QString::number(fontsize)
                                      + "px;" + "color: " + COLOR[color] + ";");
    }
}


/********************slots*********************************/

// 登录按钮（FINISHED）
void TcpClient::on_loginBtn_clicked()
{
    QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[0])->itemAt(1);
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());
    QString username =  line->text();

    item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString password = line->text();

    qDebug() << "username: " << username;
    qDebug() << "password: " << password;

    if(username == "" || password == "" ){
        errorGUI("用户名或密码不能为空");
        return;
    }

    if(username.size() > 32 || password.size() > 32){
        errorGUI("用户名或密码不能超过32字节");
        return;
    }

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
    sendPacketHead.set_function_type(PacketHead::kC2SReportLoginIn);

    sendPacketHead.set_length(64);

    /*sendClientToServerReportLogin.set_string(sendPacketHead,
        (stringPadding(QStringToString(username), 32) + stringPadding(QStringToString(password), 32)).c_str());*/
    std::string usernameString = QStringToString(username);
    std::string passwordString = QStringToString(password);

    ClientToServerReportLogin sendClientToServerReportLogin(sendPacketHead, 
        stringPadding(usernameString, 32).c_str(), stringPadding(passwordString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerReportLogin.get_string(tmpStr);
     socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    this->username = username;
    this->password = password;
}


// 注册按钮 (FINISHED)
void TcpClient::on_signupBtn_clicked()
{
    QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[0])->itemAt(1);
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());
    QString username =  line->text();

    item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString password = line->text();

    qDebug() << "username: " << username;
    qDebug() << "password: " << password;

    if(username == "" || password == "" ){
        errorGUI("用户名或密码不能为空");
        return;
    }

    if(username.size() > 32 || password.size() > 32){
        errorGUI("用户名或密码不能超过32字节");
        return;
    }

    PacketHead sendPacketHead;


    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
    sendPacketHead.set_function_type(PacketHead::kC2SReportRegister);  //注册

    sendPacketHead.set_length(64);
    /*sendClientToServerReportLogin.set_string(sendPacketHead,
        (stringPadding(QStringToString(username), 32) + stringPadding(QStringToString(password), 32)).c_str());*/

    std::string usernameString = QStringToString(username);
    std::string passwordString = QStringToString(password);

    ClientToServerReportLogin sendClientToServerReportLogin(sendPacketHead, 
        stringPadding(usernameString, 32).c_str(), stringPadding(passwordString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerReportLogin.get_string(tmpStr);
     socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;
}


// 发送消息按钮 (TODO)
void TcpClient::on_sendBtn_clicked(){
    QWidget * cur = rightStackLayout->itemAt(curIndex)->widget();
    QTextEdit* curTextEdit = static_cast<QTextEdit*>(cur->layout()->itemAt(2)->widget()->children().at(0));
    QString text =  curTextEdit->toPlainText();

    if(text == ""){
        // 空消息不必发送
        errorGUI("消息不能为空");
        return;
    }

    curTextEdit->clear();  //清空输入栏的内容

    QTextBrowser* curTextBrowser = static_cast<QTextBrowser*>(cur->layout()->itemAt(1)->widget());
    curTextBrowser->append(username + " <" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "> ");
    curTextBrowser->append(text);

    qDebug() << "当前对话的用户" << curChatter->text();
    qDebug() << "消息内容：" << text;


    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SText);
    sendPacketHead.set_function_type(PacketHead::kC2STextToUsers);

    //首先取出文本信息以便计算包长度
    std::string textString = QStringToString(text);

    //用户数量n（4字节） + 用户名（32 * n）字节 + 文本信息(string.length())
    sendPacketHead.set_length(4 + 32 * 1 + textString.length());

    //获取用户名对应的二维数组
    char **uinfo = new char*[1];
    uinfo[0] = new char[32 + 1];
    if(curChatter->text() == "群聊") {
        memset(uinfo[0], 0, 32);
    }else{
        strncpy(uinfo[0], stringPadding(QStringToString(curChatter->text()), 32).c_str(), 32);
    }


    ClientToServerTextToUsers sendClientToServerTextToUsers(sendPacketHead,
        1, uinfo, textString.c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerTextToUsers.get_string(tmpStr);
    socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    for(int i = 0; i < 1; ++i)
        delete[] uinfo[i];
    delete[] uinfo;
}


// 断线处理 (FINISHED)
void TcpClient::clientDisconnected(){
    qDebug() << "您断线了，请检查网络设置";
    errorGUI("您断线了，请检查网络设置");
}


// 修改密码按钮 (FINISHED)
void TcpClient::on_changePwdBtn_clicked(){
    changePwdWindow = new QWidget;
    changePwdWindow->setWindowTitle("修改密码");
    changePwdWindow->setFixedSize(QSize(500, 300));

    QHBoxLayout * layout0 = new QHBoxLayout;
    QLabel * usernameLabel = new QLabel("          用户名");
    QLineEdit * user = new QLineEdit;
    QRegExp rx("[^\u4e00-\u9fa5]+");
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    user->setValidator(pReg);
    layout0->addWidget(usernameLabel);
    layout0->addWidget(user);
    layout0->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout1 = new QHBoxLayout;
    QLabel * originalPwdLabel = new QLabel("     原始密码");
    QLineEdit * originalPwd = new QLineEdit;
    originalPwd->setEchoMode(QLineEdit::Password);
    layout1->addWidget(originalPwdLabel);
    layout1->addWidget(originalPwd);
    layout1->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout2 = new QHBoxLayout;
    QLabel * newPwdLabel = new QLabel("          新密码");
    QLineEdit * newPwd = new QLineEdit;
    newPwd->setEchoMode(QLineEdit::Password);
    layout2->addWidget(newPwdLabel);
    layout2->addWidget(newPwd);
    layout2->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout3 = new QHBoxLayout;
    QLabel * acknewPwdLabel = new QLabel("确认新密码");
    QLineEdit * acknewPwd = new QLineEdit;
    acknewPwd->setEchoMode(QLineEdit::Password);
    layout3->addWidget(acknewPwdLabel);
    layout3->addWidget(acknewPwd);
    layout3->setContentsMargins(50, 10, 50, 10);

    QHBoxLayout * layout4 = new QHBoxLayout;
    QPushButton * ack = new QPushButton("确认");
    QPushButton * cancel = new QPushButton("取消");
    ack->setFixedSize(QSize(75,30));
    cancel->setFixedSize(QSize(75,30));
    layout4->addWidget(ack);
    layout4->addWidget(cancel);
    layout4->setContentsMargins(50, 10, 50, 10);


    QVBoxLayout * mainlayout = new QVBoxLayout;
    mainlayout->addLayout(layout0);
    mainlayout->addLayout(layout1);
    mainlayout->addLayout(layout2);
    mainlayout->addLayout(layout3);
    mainlayout->addLayout(layout4);

    connect(ack, SIGNAL(clicked()), this, SLOT(on_changePwdAckBtn_clicked()), Qt::QueuedConnection);
    connect(cancel, SIGNAL(clicked()), this, SLOT(on_changePwdCancelBtn_clicked()), Qt::QueuedConnection);

    changePwdWindow->setLayout(mainlayout);
    changePwdWindow->show();
}


// 在修改密码界面，点击“确认”，将发送 (FINISHED)
void TcpClient::on_changePwdAckBtn_clicked(){
    QVector<QObject*> layouts = changePwdWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[0])->itemAt(1);
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());
    QString user = line->text();

    item = static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString originalPwd = line->text();

    item = static_cast<QHBoxLayout*>(layouts[2])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString newPwd = line->text();

    item = static_cast<QHBoxLayout*>(layouts[3])->itemAt(1);
    line = static_cast<QLineEdit*>(item->widget());
    QString ackNewPwd = line->text();

    if(user == "" || originalPwd == "" || newPwd == ""){
        errorGUI("用户名或密码不能为空");
        return;
    }

    if(newPwd != ackNewPwd){
        errorGUI("新密码不一致");
        return;
    }

    // TODO 发送修改密码消息
    qDebug() << "用户名" << user;
    qDebug() << "原始密码" << originalPwd;
    qDebug() << "新密码" << newPwd;
    qDebug() << "确认新密码" << ackNewPwd;

    PacketHead sendPacketHead;

    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
    sendPacketHead.set_function_type(PacketHead::kC2SReportUpdate);

    sendPacketHead.set_length(96);

    std::string userString = QStringToString(user);
    std::string originalPwdString = QStringToString(originalPwd);
    std::string newPwdString = QStringToString(newPwd);

    ClientToServerReportUpdate sendClientToServerReportUpdate(sendPacketHead,
        stringPadding(userString, 32).c_str(), stringPadding(originalPwdString, 32).c_str(), stringPadding(newPwdString, 32).c_str());

    char* tmpStr = new char[kPacketHeadLen + sendPacketHead.get_length() + 1];
    sendClientToServerReportUpdate.get_string(tmpStr);
     socket->write(tmpStr, kPacketHeadLen + sendPacketHead.get_length());

    delete[] tmpStr;

    changePwdWindow->close();
}


// 在修改密码界面，点击“取消”，将关闭窗口 (FINISHED)
void TcpClient::on_changePwdCancelBtn_clicked(){
    changePwdWindow->close();
}


// 登录界面，显示密码 (FINISHED)
void TcpClient::on_showPwdCheckBox_stateChanged(){
    QVector<QObject*> layouts = loginWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(1);
    QLayoutItem * item2 =  static_cast<QHBoxLayout*>(layouts[2])->itemAt(1);
    QCheckBox * box = static_cast<QCheckBox*>(item2->widget());
    QLineEdit* line = static_cast<QLineEdit*>(item->widget());

    if(box->isChecked()){
        line->setEchoMode(QLineEdit::Normal);
    }else{
        line->setEchoMode(QLineEdit::Password);
    }
}

 // 更新时间 （FINISHED）
void TcpClient::timeUpdate(){
    if(chatRoomWindow){
        QLineEdit* line = static_cast<QLineEdit*>(chatRoomWindow->layout()->itemAt(0)->
                                                  layout()->itemAt(5)->layout()->itemAt(1)->widget());
        line->setText(QString::number(time.elapsed() / 1000));
    }
}



// 点击用户的事件
void TcpClient::userLabelClicked(){
    QObject * object = QObject::sender();
    ClickableLabel * user = qobject_cast<ClickableLabel*>(object);
    if(preChatter){
        if(preChatter->isEnabled()){
            // 撤销高亮
            preChatter->setStyleSheet("QLabel{background: rgb(46, 50, 56);}"
                                     "QLabel{color: rgb(255, 255, 255);}"
                                     "QLabel:hover{background: rgb(58,63,69);}");
        }else{
            // 撤销高亮
            preChatter->setStyleSheet("QLabel{background: rgb(46, 50, 56);}"
                                     "QLabel{color: rgb(128, 128, 128);}"
                                     "QLabel:hover{background: rgb(58,63,69);}");
        }

    }

    if(user->isEnabled()){
        // 高亮
        user->setStyleSheet("QLabel{background: rgb(58,63,69);}"
                                 "QLabel{color: rgb(255, 255, 255);}"
                                 "QLabel:hover{background: rgb(58,63,69);}");
    } else{
        // 高亮
        user->setStyleSheet("QLabel{background: rgb(58,63,69);}"
                                 "QLabel{color: rgb(128, 128, 128);}"
                                 "QLabel:hover{background: rgb(58,63,69);}");
    }


    preChatter = user;
    curChatter->setText(user->text());

    int index = user2Index[user->text()];
    curIndex = index;
    rightStackLayout->setCurrentIndex(index);
    qDebug() << index;
    QLabel* label = static_cast<QLabel*>(rightStackLayout->currentWidget()->layout()->itemAt(0)->widget());
    label->setText(user->text());

    qDebug() << "点击了" << user->text() << " Index:" << index;
}


// 接受到包的处理，状态机 （FINISHED)
void TcpClient::readyRead(){
    qDebug() << "reading...";

    static ReadState current_read_state = READ_PACKET_HEAD;
    static QByteArray current_byte_array;
    static QByteArray set_byte_array;
    static unsigned int current_byte_num_to_read = kPacketHeadLen;
    // TODO state machine
    current_byte_array += socket->readAll();
    while(current_byte_array.size() >= current_byte_num_to_read)
    {
        if(current_byte_num_to_read)
        {
            set_byte_array = current_byte_array.left(current_byte_num_to_read);
            current_byte_array.remove(0, current_byte_num_to_read);
        }

        else
        {
            set_byte_array = "";
        }

        switch(current_read_state)
        {
            case READ_PACKET_HEAD://获得报头，将相应的字符串传进报头对应的类中，然后根据类型来做选择
                my_packet_head.set_string(set_byte_array.constData());
                switch(my_packet_head.get_packet_type())
                {
                    case PacketHead::kS2CReport:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kS2CReportSuccess:
                            case PacketHead::kS2CReportSuccessDup:
                                //登录/注册成功，进入READ_SERVER_TO_CLIENT_REPORT_SUCCESS状态准备读取后续信息
                                current_read_state = READ_SERVER_TO_CLIENT_REPORT_SUCCESS;
                                current_byte_num_to_read = my_packet_head.get_length();

                                break;
                            case PacketHead::kS2CReportUpdateSucess:
                                //更改密码成功，这个时候状态机仍然处于等待下一个packet head读入的状态
                                changePwdSuccessGUI();
                                break;
                            case PacketHead::kS2CReportWrongPwd:
                            case PacketHead::kS2CReportNoExist:
                            case PacketHead::kS2CReportMustUpdate:
                            case PacketHead::kS2CReportDuplicated:
                            case PacketHead::kS2CReportNameNotAccess:
                            case PacketHead::kS2CReportPwdNotAccess:
                            case PacketHead::kS2CReportLastPwdWrong:
                            case PacketHead::kS2CReportNowPwdNotAccess:
                                //登陆失败和注册失败，这个时候状态机仍然处于等待下一个packet head读入的状态
                                errorGUI(my_packet_head.get_function_type());
                                break;
                            default:
                                qDebug() << "switch kS2CReport my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    case PacketHead::kS2CInform:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kS2CInformOnline:
                                //提示上线,进入online状态
                                current_read_state = READ_SERVER_CLIENT_ONLINE;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kS2CInformOffline:
                                //提示下线，进入offline状态
                                current_read_state = READ_SERVER_CLIENT_OFFLINE;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            default:
                                qDebug() << "switch kS2CInform my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    case PacketHead::kS2CText:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kS2CTextSimpleText:
                                qDebug() << "get here";
                                //文本信息包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_SIMPLE_TEXT;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kS2CTextFileInfo:
                                //文本信息包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_FILE_INFO;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            /*
                                这个现在已经被notify替代了，相应的状态也不存在了
                            case PacketHead::kS2CTextFileContain:
                                //文本内容包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            */
                            case PacketHead::kS2CTextAskForClr:
                                //清屏，这个时候状态机仍然处于等待下一个packet head读入的状态
                                // cls();
                                break;// two conditions... if it is myself, yor'are out; if it is other, modify GUI
                            default:
                                qDebug() << "switch kS2CText my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    //收到用户设置包，进入相应的状态
                    case PacketHead::kS2CUserSet:
                        current_read_state = READ_SERVER_TO_CLIENT_USER_SET_UPDATE;
                        current_byte_num_to_read = my_packet_head.get_length();
                        break;
                    //在线文件通知包
                    case PacketHead::kC2CFileNotify:
                        switch(my_packet_head.get_function_type())
                        {
                            case PacketHead::kC2CFileNotifyRequest:
                                current_read_state = READ_C2C_FILE_NOTIFY_REQUEST;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kC2CFileNotifyCancelSend:
                                current_read_state = READ_C2C_FILE_NOTIFY_CANCEL_SEND;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kC2CFileNotifyAccept:
                                current_read_state = READ_C2C_FILE_NOTIFY_ACCEPT;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kC2CFileNotifyCancelRecv:
                                current_read_state = READ_C2C_FILE_NOTIFY_CANCEL_RECV;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            default:
                                qDebug() << "switch kC2CFileNotify my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    case PacketHead::kC2CFileData:
                        current_read_state = READ_C2C_FILE_DATA;
                        current_byte_num_to_read = my_packet_head.get_length();
                        break;
                    default:
                        qDebug() << "switch my_packet_head.get_packet_type() case lost";
                }
                break;
            case READ_SERVER_TO_CLIENT_REPORT_SUCCESS://报道成功,收到额外信息（见协议栈），进行进一步判断
                my_server_to_client_report_success.set_string(my_packet_head, set_byte_array.constData());
                reportSuccess();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_CLIENT_ONLINE://上线包
                my_server_to_client_inform.set_string(my_packet_head, set_byte_array.constData());
                online();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_CLIENT_OFFLINE://下线包
                my_server_to_client_inform.set_string(my_packet_head, set_byte_array.constData());
                offline();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_TO_CLIENT_TEXT_SIMPLE_TEXT://收到文本信息，将文本信息放在相应的报头中，然后调用显示文本信息
                my_server_to_client_simple_text.set_string(my_packet_head, set_byte_array.constData());
                showText();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_TO_CLIENT_TEXT_FILE_INFO://收到文件相关信息，将相应信息放入报头，然后显示文件信息,这一步只有回看的时候收到
                my_server_to_client_file_info.set_string(my_packet_head, set_byte_array.constData());
                showFileInfo();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            /*
            这一步不需要了，被notify包所取代
            case READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN://收到文件内容信息，将相应信息放入报头，然后进行下载操作
                my_server_to_client_text_file_contain.set_string(my_packet_head, set_byte_array.constData());
                writeFileContain();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            */
            
            case READ_SERVER_TO_CLIENT_USER_SET_UPDATE://收到设置用户信息，将相应信息放入报头，然后做相应的设置操作
                my_server_to_client_user_set_update.set_string(my_packet_head, set_byte_array.constData());
                setConfig();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_C2C_FILE_NOTIFY_REQUEST://recv收到请求用户发送包
                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
                showTryToSend();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            
            case READ_C2C_FILE_NOTIFY_ACCEPT://sender收到同意请求包
                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
                sendFileData();//向recv发送数据包
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_C2C_FILE_DATA://recv收到数据包
                my_sender_to_receiver_file_data.set_string(my_packet_head, set_byte_array.constData());
                writeDataAndRequest();//写数据并发送请求用户发送包
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_C2C_FILE_NOTIFY_CANCEL_RECV://send收到取消接收包
                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
                cancelSendFileDataPassive();//取消发送
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;

            case READ_C2C_FILE_NOTIFY_CANCEL_SEND://recv收到取消发送包
                my_sender_to_receiver_file_notify.set_string(my_packet_head, set_byte_array.constData());
                cancelRecvFileDataPassive();//取消接收
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            
            default:
                qDebug() << "switch current_read_state case lost";
        }
    }
}


void TcpClient::on_fileDialogBtn_clicked(){
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Open Document",
                QDir::currentPath(),
                 tr("Allfile(*.*)")
                );

//    qDebug() << filename;
    int index = filename.lastIndexOf('/');
    filename = filename.mid(index+1, -1);
    tryToSend(filename);
}

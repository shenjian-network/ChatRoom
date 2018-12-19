#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSize>
#include <QMessageBox>
#include <QPainter>
#include <QDir>
#include <QTextBrowser>
#include <QStackedLayout>
#include <QCheckBox>

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
    layout1->addWidget(unameLabel);
    layout1->addWidget(unameEdit);
    layout1->setContentsMargins(30, 10, 30, 10);

    QHBoxLayout * layout2 = new QHBoxLayout;
    QLabel *upwdLabel = new QLabel("密码");
    QLineEdit *upwdEdit = new QLineEdit();
    upwdEdit->setEchoMode(QLineEdit::Password);
    unameLabel->setBuddy(upwdEdit);
    layout2->addWidget(upwdLabel);
    layout2->addWidget(upwdEdit);
    layout2->setContentsMargins(30, 10, 30, 10);

    QHBoxLayout * layout3 = new QHBoxLayout;
    QPushButton * login = new QPushButton("登录");
    QPushButton * signup = new QPushButton("注册");
    layout3->addWidget(login);
    layout3->addWidget(signup);
    layout3->setContentsMargins(100, 10, 100, 10);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);
    mainLayout->addLayout(layout3);
    mainLayout->setMargin(50);

    connect(login, SIGNAL(clicked()), this, SLOT(on_loginBtn_clicked()));
    connect(signup, SIGNAL(clicked()), this, SLOT(on_signupBtn_clicked()));

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



// 需要读包，获取错误信息
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



void TcpClient::chatRoomGUI(){
    chatRoomWindow = new QWidget;
    chatRoomWindow->setWindowTitle("聊天室");

    QHBoxLayout * layout1 = new QHBoxLayout;
    QTextBrowser * textfield = new QTextBrowser;
    userList = new QListWidget;
    textfield->setMaximumSize(500, 400);
    textfield->setMinimumSize(500, 400);
    textfield->setHtml("<h1>This is a test</h1>");
    textfield->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    insertListWidget("zhongyuchen");
    userList->setMaximumSize(200, 400);
    userList->setMinimumSize(200, 400);
    layout1->addWidget(textfield);
    layout1->addWidget(userList);

    QHBoxLayout * layout2 = new QHBoxLayout;
//    layout2->setStackingMode(QStackedLayout::StackAll);
    QTextEdit * textedit = new QTextEdit;
    textedit->setMaximumSize(700, 300);
    textedit->setMinimumSize(700, 300);
    textedit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    layout2->addWidget(textedit);

    QPushButton * send = new QPushButton("发送");
    send->setMaximumSize(50, 20);
    send->setMinimumSize(50, 20);
    send->setGeometry(640, 670, 50, 20);
    layout2->addWidget(send);

    QVBoxLayout * mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout1);
    mainLayout->addLayout(layout2);


    connect(send, SIGNAL(clicked()), this, SLOT(on_sendBtn_clicked()));

    chatRoomWindow->setLayout(mainLayout);
    chatRoomWindow->resize(700, 700);
    chatRoomWindow->show();
}



void TcpClient::changePwdGUI(){

}

void TcpClient::configGUI(){

}

// two conditions... if it is myself, yor'are out; if it is other, modify GUI
void TcpClient::offline(){

}

// only one condition
void TcpClient::online(){

}

void TcpClient::cls(){

}

void TcpClient::showText(){

}

// read config
void TcpClient::setConfig(){

}

void TcpClient::insertListWidget(QString desc){
    QListWidgetItem * item = new QListWidgetItem;
    QCheckBox * box = new QCheckBox(desc);
    box->setCheckable(true);
    userList->addItem(item);
    userList->setItemWidget(item, box);
}

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

void TcpClient::getCheckState(){
    int count = userList->count();
    bool isChecked = true;
    QVector<bool> vecIsChecked;
    vecIsChecked.clear();
    QListWidgetItem * item;
    QWidget * widget;
    QCheckBox * box;
    QString text;
    for(int i = 0;i < count; ++i){
        item = userList->item(i);
        widget = userList->itemWidget(item);
        box = static_cast<QCheckBox*>(widget);
        text = box->text();
        isChecked = box->isChecked();
        vecIsChecked.push_back(isChecked);
    }
}

// 更改密码成功GUI
void TcpClient::changePwdSuccessGUI(){

}


void TcpClient::showFileInfo(){

}

void TcpClient::writeFileContain(){

}



void TcpClient::reportSuccess(){
    auto length = my_server_to_client_report_success.get_packet_head().get_length();

    // 注册成功包
    if(length == 0){
        // TODO 注册成功
    }
    else{ // 登录成功包
        loginWindow->hide();
        chatRoomGUI();
    }
}




std::string QStringToString(const QString & myQstring)
{
    return std::string(myQstring.toLocal8Bit().constData());
}

std::string & stringPadding(std::string && myString, const unsigned int & len)
{
    myString.resize(len, 0);
    return myString;
}





/********************slots*********************************/

/* 点击登录按钮的槽函数
 */
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
        QMessageBox *errorBox = new QMessageBox();
        errorBox->setWindowTitle("错误");
        errorBox->setText("用户名或密码不能为空!");
        errorBox->show();
        return;
    }

    if(username.size() > 32 || password.size() > 32){
        QMessageBox *errorBox = new QMessageBox();
        errorBox->setWindowTitle("错误");
        errorBox->setText("用户名或密码不能超过32字节!");
        errorBox->show();
        return;
    }

    PacketHead sendPacketHead;


//    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
//    sendPacketHead.set_function_type(PacketHead::kC2SReportLoginIn);

    sendPacketHead.set_length(64);

    ClientToServerReportLogin sendClientToServerReportLogin;

    sendClientToServerReportLogin.set_string(sendPacketHead,
        (stringPadding(QStringToString(username), 32) + stringPadding(QStringToString(password), 32)).c_str());

    
    char* tmpStr = new char[sendPacketHead.get_length()];
    sendClientToServerReportLogin.get_string(tmpStr);
    socket->write(tmpStr, 8 + sendPacketHead.get_length());
    delete[] tmpStr;

}


/*   需要向服务器发送包 */
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
        QMessageBox *errorBox = new QMessageBox();
        errorBox->setWindowTitle("错误");
        errorBox->setText("用户名或密码不能为空!");
        errorBox->show();
        return;
    }

    if(username.size() > 32 || password.size() > 32){
        QMessageBox *errorBox = new QMessageBox();
        errorBox->setWindowTitle("错误");
        errorBox->setText("用户名或密码不能超过32字节!");
        errorBox->show();
        return;
    }

    PacketHead sendPacketHead;


//    sendPacketHead.set_packet_type(PacketHead::kC2SReport);
//    sendPacketHead.set_function_type(PacketHead::kC2SReportLoginIn);

    sendPacketHead.set_length(64);

    ClientToServerReportLogin sendClientToServerReportLogin;

    sendClientToServerReportLogin.set_string(sendPacketHead,
        (stringPadding(QStringToString(username), 32) + stringPadding(QStringToString(password), 32)).c_str());


    char* tmpStr = new char[sendPacketHead.get_length()];
    sendClientToServerReportLogin.get_string(tmpStr);
    socket->write(tmpStr, 8 + sendPacketHead.get_length());
    delete[] tmpStr;
}

void TcpClient::on_sendBtn_clicked(){
    qDebug() << "it works";

    QVector<QObject*> layouts = chatRoomWindow->layout()->children().toVector();
    QLayoutItem * item =  static_cast<QHBoxLayout*>(layouts[1])->itemAt(0);
    QTextEdit* line = static_cast<QTextEdit*>(item->widget());
    QString text =  line->toPlainText();
    line->clear();

    item =  static_cast<QHBoxLayout*>(layouts[0])->itemAt(0);
    line = static_cast<QTextBrowser*>(item->widget());
    line->append(text);

    // 还需要向服务器发啊
}


void TcpClient::disconnected(){

}




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
                                //文本信息包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_SIMPLE_TEXT;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kS2CTextFileInfo:
                                //文本信息包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_FILE_INFO;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kS2CTextFileContain:
                                //文本内容包
                                current_read_state = READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN;
                                current_byte_num_to_read = my_packet_head.get_length();
                                break;
                            case PacketHead::kS2CTextAskForClr:
                                //清屏，这个时候状态机仍然处于等待下一个packet head读入的状态
                                cls();
                                break;
                            default:
                                qDebug() << "switch kS2CText my_packet_head.get_packet_type() case lost";
                        }
                        break;
                    case PacketHead::kS2CUserSet:
//                        current_read_state = PacketHead::kS2CUserSetUpdate;
                        current_byte_num_to_read = my_packet_head.get_length();
                        setConfig();
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
            case READ_SERVER_TO_CLIENT_TEXT_FILE_INFO://收到文件相关信息，将相应信息放入报头，然后显示文件信息
                my_server_to_client_file_info.set_string(my_packet_head, set_byte_array.constData());
                showFileInfo();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_TO_CLIENT_TEXT_FILE_CONTAIN://收到文件内容信息，将相应信息放入报头，然后进行下载操作
                my_server_to_client_text_file_contain.set_string(my_packet_head, set_byte_array.constData());
                writeFileContain();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            case READ_SERVER_TO_CLIENT_USER_SET_UPDATE://收到设置用户信息，将相应信息放入报头，然后做相应的设置操作
                my_server_to_client_user_set_update.set_string(my_packet_head, set_byte_array.constData());
                setConfig();
                current_read_state = READ_PACKET_HEAD;
                current_byte_num_to_read = kPacketHeadLen;
                break;
            default:
                qDebug() << "switch current_read_state case lost";
        }
    }
}


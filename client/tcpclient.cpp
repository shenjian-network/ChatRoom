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
    delete socket;
}


//ui->upwdEdit->setEchoMode(QLineEdit::Password);

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
void TcpClient::errorGUI(){
    QMessageBox *errorBox = new QMessageBox();
    // TODO: 读包

    errorBox->setText("错误！");
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




/****slots****/

void TcpClient::readyRead(){
    qDebug() << "reading...";

    char packet_head[8 * 8]; // 8B
    socket->read(packet_head, 8*8);

    // TODO state machine
}

void TcpClient::disconnected(){

}


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

    if(username == "" || password == ""){
        QMessageBox *errorBox = new QMessageBox();
        errorBox->setWindowTitle("错误");
        errorBox->setText("用户名或密码不能为空!");
        errorBox->show();
    }

    // TODO: 发包

    // loginWindow->hide();
    //errorGUI();

    // TEST CODE:
    chatRoomGUI();
    loginWindow->hide();
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

    if(username == "" || password == ""){
        QMessageBox *errorBox = new QMessageBox();
        errorBox->setWindowTitle("错误");
        errorBox->setText("用户名或密码不能为空!");
        errorBox->show();
    }

    // TODO: 发包

    // loginWindow->hide();
    //errorGUI();
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


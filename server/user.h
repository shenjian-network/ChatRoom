//Copyright 2018 Tongji
//License:GPL
//Author:zyc lsx
//This is a User class used to connect with database and create packet.
#ifndef MYUSER
#define MYUSER
#include "server_to_client.h"
#include "client_to_server.h"
#include "server_to_database.h"
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include<netinet/tcp.h>
#include <signal.h>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;
/*提供socket的一些公共函数*/
class SocketFunction{
public: 
    static void SetSockNonBlock(int sock);
    static int InitSocket();
    static int MyRecvC(const int& sock,const int& required_size,char* buffer);
    static const int my_port;
    static const char* my_ip;
private: 
    static void Die(const char*msg);
};
/*封装用户的状态信息*/
class User{
public: 
    User();
    User(const char *uname);
    char* get_user_name();
    bool get_user_status();
    int get_user_sockfd();
    void set_user_status(bool b);
    void set_user_sockfd(int i);
    void set_user_name(const char* n);
    /*请自行释放char*空间，以下同该函数*/
    bool SendMessage(ServerToClientBase* scb);//返回false，表示链接断开/堵塞，其他表示发送成功。
    void Clear();
    friend bool operator==(const User& ua,const User& ub);
    friend bool operator==(const User& ua,const char* cb);
private: 
    static const int error_max_number=3;
    char name[33];      //用户名
    bool online;         //0表示不在线，1表示在线
    int sock_fd;        //用于连接的socket标识符,范围0-1023可直接映射
};
/*程序主体，管理所有用户*/
class UserManager{
public:
    typedef User LinkUnit;
    UserManager();
    ~UserManager();
    void ProvideService();
    //void SignIn(const char *name, const char *passwd, int fd);  //进行一次登录操作
    //void SignOut();                                             //判断是否有用户下线
    //void SignUp(const char *name, const char *passwd, int fd);  //用户注册
    //void ChangePwd(const char *name, const char *pwd_old, const char *pwd_new);
private:
    //该部分创建的包用完后需要手动释放
    ServerToClientBase* CreateRetReportLoginPacket(ClientToServerReportLogin* cl,char* set_contain,unsigned long& set_length);//包含登录和注册两部分
    ServerToClientBase* CreateRetReportUpdatePacket(ClientToServerReportUpdate* cu);//包含改密部分
    ServerToClientBase* CreateRetSetPacket(const char* ftname,char* set_contain,unsigned long& set_length);//只有在登录成功才会使用该配置包
    ServerToClientBase* CreateRetInformPacket(const bool& is_online,const char* fname);
    ServerToClientBase* CreateRetTextContain(ClientToServerTextToUsers* ct,const char* fname);
    ServerToClientBase* CreateRetFileInfo(ClientToServerTextFileToUsers* cfi,const char* fname);
    ServerToClientBase* CreateRetFileContain(ClientToServerTextAskForFile* cfc,const char* fname);
    ServerToClientBase* CreateRetCls();
    ServerToClientBase** CreateRetAskForTexts(int& real_num,ClientToServerTextAskForTexts* cts,const char* uname);//返回文本消息或者文件info的数组，大小为real_num,该部分用delete[]
    void CreateRetSetPacket(ClientToServerUserSetUpdate* cs,const char* fname);
    //获得系统时间
    static void get_now_time(char *&bufferWrite);
    //合法性检测
    bool CheckStr(const char* s);
    //Report
    bool ClientSign(const int& client,const PacketHead& p,const char* b);
    bool ClientRegister(const int& client,const PacketHead& p,const char* b);
    bool ClientUpdate(const int& client,const PacketHead& p,const char* b);
    //Text
    bool ClientTextToUsers(const int& client,const PacketHead& p,const char* b);
    bool ClientFileToUsers(const int& client,const PacketHead& p,const char* b);
    bool ClientTextAskForFile(const int& client,const PacketHead& p,const char* b);
    bool ClientTextAskForTexts(const int& client,const PacketHead& p,const char* b);
    //Set
    void ClientUserSetUpdate(const int& client,const PacketHead& p,const char* b);
    //Close
    void ClientClose(const int& client);
    //数据成员
    vector<User> my_clients;
    LinkUnit* my_links;
    int server_sock;
    ServerToDatabase mydb;
};

#endif
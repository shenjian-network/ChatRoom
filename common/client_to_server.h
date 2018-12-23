//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is packet client send to server.
#ifndef Client2Server
#define Client2Server
#include "packet_head.h"
/*因为部分存在着指针，因而不能使用赋值操作，而且由于部分有new和delete，为简化设计因而取消其他set,
  一切只能通过初始化加参数或者初始化不加参数但是set_string(二者只能取其一)完成赋值*/
/*基类*/
class ClientToServerBase{
public: 
    ClientToServerBase();
    ClientToServerBase(const PacketHead& ph);
    virtual ~ClientToServerBase(){}
    /*set_string可以将网络序的规范字符串转为该类（主机序）内部存储形式*/
    /*get_string可以将该类内部存储（主机序）改为网络序的规范字符串*/
    /*其他均为主机序的自定义设置和获取*/
    virtual void get_string(char* s);
    PacketHead get_packet_head();
    virtual void set_string(const PacketHead& ph,const char* s);//包含自身的报头，和后面收到的length的字符串
private:
    PacketHead my_head; //报头共8字节
};
/*报道包：登录和注册*/
class ClientToServerReportLogin:public ClientToServerBase
{
public:
    ClientToServerReportLogin();
    ClientToServerReportLogin(const PacketHead& ph,const char* uname,const char* upwd);
    ~ClientToServerReportLogin(){}
    void get_string(char* s);
    char* get_user_name();
    char* get_user_pwd();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //packetHead myHead;
    char user_name[33];
    char user_pwd[33];//密码
};
/*报道包：改密*/
class ClientToServerReportUpdate:public ClientToServerBase
{
public:
    ClientToServerReportUpdate();
    ClientToServerReportUpdate(const PacketHead& ph,const char* uname,const char* last_upwd,const char* now_upwd);
    virtual ~ClientToServerReportUpdate(){}
    void get_string(char* s);
    char* get_user_name();
    char* get_last_user_pwd();
    char* get_now_user_pwd();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //packetHead myHead;
    char user_name[33];
    char last_user_pwd[33];//原密码
    char now_user_pwd[33];//现密码
};
/*文本信息发送：给一个或多个发文本信息*/
class ClientToServerTextToUsers:public ClientToServerBase
{
public:
    ClientToServerTextToUsers();
    ClientToServerTextToUsers(const PacketHead& ph,const int& unum,char** uinfo,const char* cinfo);
    virtual ~ClientToServerTextToUsers();
    void get_string(char* s);
    unsigned int get_user_num();
    char** get_user_info();
    char* get_text_info();
    int get_text_length();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //packetHead myHead;
    unsigned int user_num;//该部分决定申请空间的大小
    char** user_info;//该部分为二维数组,形式为user_info[user_num][33]
    char* text_info;//该部分大小取决于packet中的length
    int text_length;//文本的长度
};
/*文本信息的发送：给一个或多个发文件信息*/
class ClientToServerTextFileToUsers:public ClientToServerBase
{
public:
    ClientToServerTextFileToUsers();
    ClientToServerTextFileToUsers(const PacketHead& ph,const int& unum,char** uinfo,const char* cinfo,const char* fname);
    virtual ~ClientToServerTextFileToUsers();
    void get_string(char* s);
    char* get_file_name();
    unsigned int get_user_num();
    char** get_user_info();
    char* get_text_info();
    int get_text_length();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //packetHead myHead;
    unsigned int user_num;//该部分决定申请空间的大小
    char** user_info;//该部分为二维数组,形式为user_info[user_num][33]
    char* text_info;//该部分大小取决于packet中的length
    char file_name[65];
    int text_length;
};
/*文本信息的发送：请求给自己发送文件内容*/
class ClientToServerTextAskForFile:public ClientToServerBase
{
public:
    ClientToServerTextAskForFile();
    ClientToServerTextAskForFile(const PacketHead& ph,const int& fkey);
    virtual ~ClientToServerTextAskForFile(){}
    void get_string(char* s);
    unsigned int get_file_key();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //packetHead myHead;
    unsigned int file_key;
};
/*文本信息的发送：请求回看n条信息*/
class ClientToServerTextAskForTexts:public ClientToServerBase
{
public:
    ClientToServerTextAskForTexts();
    ClientToServerTextAskForTexts(const PacketHead& ph,const int& lnum,const char* ouser);
    virtual ~ClientToServerTextAskForTexts(){}
    void get_string(char* s);
    unsigned int get_list_num();
    char* get_op_user();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //packetHead myHead;
    unsigned int list_num;
    char op_user[32];
};
/*更改设置*/
class ClientToServerUserSetUpdate:public ClientToServerBase
{
public:
    ClientToServerUserSetUpdate();
    ClientToServerUserSetUpdate(const PacketHead& ph,const char* sc);
    virtual ~ClientToServerUserSetUpdate();
    void get_string(char* s);
    char* get_user_set_data();
    int get_text_length();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //packetHead myHead;
    char* user_set_data;
};
/*一对一在线传文件*/
/*通知包*/
class SenderToReceiverFileNotify:public ClientToServerBase{
public: 
    SenderToReceiverFileNotify();
    SenderToReceiverFileNotify(const PacketHead& ph,const char* sn,const char* rn,const char* fn,unsigned int fs);
    virtual void get_string(char* s);//need 140
    char* get_sender_name();
    char* get_receiver_name();
    char* get_file_name();
    unsigned int get_file_size();
    unsigned int get_block_num();
    virtual void set_string(const PacketHead& ph,const char* s);
private:
    //packetHead myHead;
    char sender_name[32];
    char receiver_name[32];
    char file_name[64];
    unsigned int file_size;// as block_num
};
/*数据包*/
class SenderToReceiverFileData:public SenderToReceiverFileNotify{
public: 
    SenderToReceiverFileData();
    SenderToReceiverFileData(const PacketHead& ph,const char* sn,const char* rn,const char* fn,unsigned int bn,const char* fc);
    char* get_file_contain();
    void get_string(char* s);// need 4096
    void set_string(const PacketHead& ph,const char* s);
private: 
    //packetHead myHead;
    // char sender_name[32];
    // char receiver_name[32];
    // char file_name[64];
    // unsigned int file_size;// as block_num
    char file_contain[FILEBUFFERSIZE];
};
#endif
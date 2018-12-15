//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is packet client send to server.
#ifndef Client2Server
#define Client2Server
#include "packet_head.h"
/*基类*/
class ClientToServerBase{
public: 
    ClientToServerBase(const PacketHead& ph);
    /*set_string可以将网络序的规范字符串转为该类（主机序）内部存储形式*/
    /*get_string可以将该类内部存储（主机序）改为网络序的规范字符串*/
    /*其他均为主机序的自定义设置和获取*/
    void get_string(char* s);
    PacketHead get_packet_head();
    void set_string(const char* s);
    void set_packet_head(const PacketHead& ph); 
private:
    PacketHead my_head; //报头共8字节
};
/*因为部分存在着指针的copy，因而不能使用赋值操作，而且由于部分有new和delete，为简化设计因而取消其他set,一切只能通过初始化或者set_string完成赋值*/
/*报道包：登录和注册*/
class ClientToServerReportLogin:public:ClientToServerBase
{
public:
    ClientToServerReportLogin(const PacketHead& ph,const char* uname,const char* upwd);
    void get_string(char* s);
    char* get_user_name();
    char* get_user_pwd();
    void set_string(const char* s);  
private:
    //packetHead myHead;
    char user_name[33];
    char user_pwd[33];//密码
};
/*报道包：改密*/
class ClientToServerReportUpdate:public:ClientToServerBase
{
public:
    ClientToServerReportUpdate(const PacketHead& ph,const char* uname,const char* last_upwd,const char* now_upwd);
    void get_string(char* s);
    char* get_user_name();
    char* get_last_user_pwd();
    char* get_now_user_pwd();
    void set_string(const char* s);  
private:
    //packetHead myHead;
    char user_name[33];
    char last_user_pwd[33];//原密码
    char now_user_pwd[33];//现密码
};
/*文本信息发送：给一个或多个发文本信息*/
class ClientToServerTextToUsers:public:ClientToServerBase
{
public:
    ClientToServerTextToUsers(const PacketHead& ph,const int& unum,const char** uinfo,const char* cinfo);
    ~ClientToServerTextToUsers();
    void get_string(char* s);
    unsigned int get_user_num();
    char** get_user_info();
    char* get_text_info();
    void set_string(const char* s);  
private:
    //packetHead myHead;
    unsigned int user_num;//该部分决定申请空间的大小
    char** user_info;//该部分为二维数组,形式为user_info[user_num][33]
    char* text_info;//该部分大小取决于packet中的length
};
/*文本信息的发送：给一个或多个发文件信息*/
class ClientToServerTextAskForFile:public:ClientToServerTextToUsers
{
public:
    ClientToServerTextAskForFile(const PacketHead& ph,const int& unum,const char** uinfo,const char* cinfo,const char* fname);
    ~ClientToServerTextAskForFile();
    void get_string(char* s);
    char* get_file_name();
    void set_string(const char* s);  
private:
    //packetHead myHead;
    //unsigned int user_num;//该部分决定申请空间的大小
    //char** user_info;//该部分为二维数组,形式为user_info[user_num][33]
    //char* text_info;//该部分大小取决于packet中的length
    char file_name[65];
};
/*文本信息的发送：请求给自己发送文件内容*/
class ClientToServerTextAskForFile:public:ClientToServerBase
{
public:
    ClientToServerTextAskForFile(const PacketHead& ph,const int& fkey);
    void get_string(char* s);
    unsigned int get_file_key();
    void set_string(const char* s);  
private:
    //packetHead myHead;
    unsigned int file_key;
};
/*文本信息的发送：请求回看n条信息*/
class ClientToServerTextAskForTexts:public:ClientToServerBase
{
public:
    ClientToServerTextAskForTexts(const PacketHead& ph,const int& lnum);
    void get_string(char* s);
    unsigned int get_list_num();
    void set_string(const char* s);  
private:
    //packetHead myHead;
    unsigned int list_num;
};
/*更改设置*/
class ClientToServerUserSetUpdate:public:ClientToServerBase
{
public:
    ClientToServerUserSetUpdate(const PacketHead& ph,const char* sc);
    ~ClientToServerUserSetUpdate();
    void get_string(char* s);
    char* get_user_set_data();
    void set_string(const char* s);  
private:
    //packetHead myHead;
    char* user_set_data;
};
#endif
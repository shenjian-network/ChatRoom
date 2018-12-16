//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is packet server send to client. 
#ifndef Server2Client
#define Server2Client
#include "packet_head.h"
/*因为部分存在着指针，因而不能使用赋值操作，而且由于部分有new和delete，为简化设计因而取消其他set,
  一切只能通过初始化加参数或者初始化不加参数但是set_string(二者只能取其一)完成赋值*/
/*基类*/
class ServerToClientBase{
public: 
    ServerToClientBase();
    ServerToClientBase(const PacketHead& ph);
    /*set_string可以将网络序的规范字符串转为该类（主机序）内部存储形式*/
    /*get_string可以将该类内部存储（主机序）改为网络序的规范字符串*/
    /*其他均为主机序的自定义设置和获取*/
    void get_string(char* s);
    PacketHead get_packet_head();
    void set_string(const PacketHead& ph,const char* s);//包含自身的报头，和后面收到的length的字符串
private:
    PacketHead my_head; //报头共8字节
};
/*报道包：包含成功的额外信息*/
class ServerToClientReportSuccess:public ServerToClientBase
{
public:
    ServerToClientReportSuccess();
    ServerToClientReportSuccess(const PacketHead& ph,char*ltime,int unum,char**ustatus);
    ~ServerToClientReportSuccess();
    void get_string(char* s);
    char* get_last_login_time();
    unsigned int get_user_num();
    char** get_user_status();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char last_login_time[20];//上次登录时间
    unsigned int user_num;//用户总数
    char** user_status;//所有用户状态信息：32+1，具体用法为 user_status[user_num][33]
};
/*通知包：包含上线和下线信息*/
class ServerToClientInform:public ServerToClientBase
{
public:
    ServerToClientInform();
    ServerToClientInform(const PacketHead& ph,char*uname);
    void get_string(char* s);
    char* get_user_name();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char user_name[33];
};
/*文本信息发送：给一个用户发送文本消息*/
class ServerToClientTextSimpleText:public ServerToClientBase
{
public:
    ServerToClientTextSimpleText();
    ServerToClientTextSimpleText(const PacketHead& ph,char*scontain);
    ~ServerToClientTextSimpleText();
    void get_string(char* s);
    char* get_simple_text_contain();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char* simple_text_contain;
};
/*文本信息发送：给一个用户发送文件信息*/
class ServerToClientTextFileInfo:public ServerToClientBase
{
public:
    ServerToClientTextFileInfo();
    ServerToClientTextFileInfo(const PacketHead& ph,char*fname,unsigned int fkey);
    virtual ~ServerToClientTextFileInfo(){}
    void get_string(char* s);
    char* get_file_name();
    unsigned int get_file_key();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char file_name[65];
    unsigned int file_key;
};
/*文本信息发送：给一个用户发送文件内容*/
class ServerToClientTextFileContain:public ServerToClientTextFileInfo
{
public:
    ServerToClientTextFileContain();
    ServerToClientTextFileContain(const PacketHead& ph,char*fname,unsigned int fkey,char* fcontain);
    ~ServerToClientTextFileContain();
    void get_string(char* s);
    char* get_file_contain();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    //char file_name[65];
    //unsigned int file_key;
    char* file_contain;
};
/*设置包：命令用户更改个性化设置*/
class ServerToClientUserSetUpdate:public ServerToClientBase
{
public:
    ServerToClientUserSetUpdate();
    ServerToClientUserSetUpdate(const PacketHead& ph,char*sc);
    ~ServerToClientUserSetUpdate();
    void get_string(char* s);
    char* get_user_set_data();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char* user_set_data;
};
#endif
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
    virtual ~ServerToClientBase(){}
    /*set_string可以将网络序的规范字符串转为该类（主机序）内部存储形式*/
    /*get_string可以将该类内部存储（主机序）改为网络序的规范字符串*/
    /*其他均为主机序的自定义设置和获取*/
    virtual void get_string(char* s);
    PacketHead get_packet_head();
    virtual void set_string(const PacketHead& ph,const char* s);//包含自身的报头，和后面收到的length的字符串
private:
    PacketHead my_head; //报头共8字节
};
/*报道包：包含成功的额外信息*/
class ServerToClientReportSuccess:public ServerToClientBase
{
public:
    ServerToClientReportSuccess();
    ServerToClientReportSuccess(const PacketHead& ph,const char*ltime,const int unum,char**ustatus);
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
    ServerToClientInform(const PacketHead& ph,const char*uname);
    ~ServerToClientInform(){}
    void get_string(char* s);
    char* get_user_name();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char user_name[33];
};
/*文本信息发送：文本基类*/
class ServerToClientText:public ServerToClientBase
{
public:
    ServerToClientText();
    ServerToClientText(const PacketHead& ph,const char*frname,const char* toname,const char*ntime);
    ~ServerToClientText(){}
    void get_string(char* s);
    char* get_user_from_name();
    char* get_user_to_name();
    char* get_now_time();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char user_from_name[33];
    char user_to_name[33];
    char now_time[20];
};
/*文本信息发送：给一个用户发送文本消息*/
class ServerToClientTextSimpleText:public ServerToClientText
{
public:
    ServerToClientTextSimpleText();
    ServerToClientTextSimpleText(const PacketHead& ph,const char*frname,const char* toname,const char*ntime,const char*scontain);
    ~ServerToClientTextSimpleText();
    void get_string(char* s);
    char* get_simple_text_contain();
    int get_text_length();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    //char user_from_name[33];
    //char user_to_name[33];
    //char now_time[20];
    char* simple_text_contain;
    int text_length;//该部分对于用户隐藏
};
/*文本信息发送：给一个用户发送文件信息*/
class ServerToClientTextFileInfo:public ServerToClientText
{
public:
    ServerToClientTextFileInfo();
    ServerToClientTextFileInfo(const PacketHead& ph,const char*frname,const char* toname,const char*ntime,const char*fname,const unsigned int fkey);
    ~ServerToClientTextFileInfo(){}
    void get_string(char* s);
    char* get_file_name();
    unsigned int get_file_key();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    //char user_from_name[33];
    //char user_to_name[33];
    //char now_time[20];
    char file_name[65];
    unsigned int file_key;
};
/*文本信息发送：给一个用户发送文件内容*/
class ServerToClientTextFileContain:public ServerToClientTextFileInfo
{
public:
    ServerToClientTextFileContain();
    ServerToClientTextFileContain(const PacketHead& ph,const char*frname,const char* toname,const char*ntime,const char*fname,const unsigned int fkey,const char* fcontain);
    ~ServerToClientTextFileContain();
    void get_string(char* s);
    char* get_file_contain();
    int get_text_length();
    void set_string(const PacketHead& ph,const char* s); 

private:
    //PacketHead my_head;
    //char user_from_name[33];
    //char user_to_name[33];
    //char now_time[20];
    //char file_name[65];
    //unsigned int file_key;
    char* file_contain;
    int text_length;//
};
/*设置包：命令用户更改个性化设置*/
class ServerToClientUserSetUpdate:public ServerToClientBase
{
public:
    ServerToClientUserSetUpdate();
    ServerToClientUserSetUpdate(const PacketHead& ph,const char*sc);
    ~ServerToClientUserSetUpdate();
    void get_string(char* s);
    char* get_user_set_data();
    int get_text_length();
    void set_string(const PacketHead& ph,const char* s);  
private:
    //PacketHead my_head;
    char* user_set_data;
};
#endif
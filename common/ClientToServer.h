#ifndef Client2Server
#define Client2Server
#include "packetHead.h"
/*报道包：包含登录和注册*/
class C2S_LoginUser{
    packetHead myHead; //报头共8字节
    string Uname; // 名称,填充后32字节，加上尾零33字节
    string Upwd; //明文密码，填充后32字节，加上尾零33字节
public: 
    /*以下部分输入输出均按照含尾零处理(因为是string)，且输入内容为网络序，而输出内容均为主机序*/
    unsigned short getPacketType();
    unsigned short getFunctionType();
    unsigned int getLength();
    string getUname();
    string getUpwd();
    void setPacketType(unsigned short);
    void setFunctionType(unsigned short);
    void setLength(unsigned int);   
    void setUname(string);
    void setUpwd(string);
    void getString(string);
    string toString();
};
/*报道包：改密*/
class C2S_UpdateUser:public:C2S_LoginUser
{
    //packetHead myHead;
    //char Uname[33];
    //char lastUpwd[33];//原密码
    string nowUpwd;//现密码
public:
    string getLastUpwd();
    void setLastUpwd(string);
    string getNowUpwd();
    void setNowUpwd(string);
    void getString(string);
    string toString();   
};
#endif
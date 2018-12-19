//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is packet head.
#ifndef PACKETHEAD 
#define PACKETHEAD
#include<stdio.h>
#include<arpa/inet.h>
#include<string.h>
#include<iostream>
#include<string>
/*为了方便统一，因而只有getString和toString才包括网络序和主机序的转化，其他均为正常用法*/
class PacketHead{
public:
    PacketHead();
    PacketHead(const unsigned short s1,const unsigned short s2,const unsigned int i1);
    /*set_string可以将网络序的规范字符串转为该类（主机序）内部存储形式*/
    /*get_string可以将该类内部存储（主机序）改为网络序的规范字符串*/
    /*其他均为主机序的自定义设置和获取*/
    unsigned short get_packet_type();
    unsigned short get_function_type();
    unsigned int get_length()const;
    void get_string(char* s);
    void set_packet_type(const unsigned short us);
    void set_function_type(const unsigned short us);
    void set_length(const unsigned int ui);
    void set_string(const char* s);
    /*packet_type*/
    /*Client to Server*/
    static const unsigned short kC2SReport=0x10;//报道包
    static const unsigned short kC2SText=0x12;//文本信息发送包
    static const unsigned short kC2SUserSet=0x13;//设置包
    /*Server to Client*/
    static const unsigned short kS2CReport=0x80;//报道包
    static const unsigned short kS2CInform=0x81;//上线&下线包
    static const unsigned short kS2CText=0x82;//文本信息反馈包
    static const unsigned short kS2CUserSet=0x83;//设置包
    /*function_type*/
    /*Client to Server*/
    static const unsigned short kC2SReportLoginIn=0x0;//登录
    static const unsigned short kC2SReportRegister=0x1;//注册
    static const unsigned short kC2SReportUpdate=0x2;//更新
    static const unsigned short kC2STextToUsers=0x0;//给1个或多个文本信息
    static const unsigned short kC2STextFileToUsers=0x2;//给1个或多个发文件信息 
    static const unsigned short kC2STextAskForFile=0x4;//请求给自己发文件内容 
    static const unsigned short kC2STextAskForTexts=0x5;//请求回看n条信息
    static const unsigned short kC2SUserSetUpdate=0x0;//更改设置请求
    /*Server to Client*/
    static const unsigned short kS2CReportSuccess=0x0;//登录/注册成功
    static const unsigned short kS2CReportSuccessDup=0x1;//登录成功（但是踢了原来登陆的人)
    static const unsigned short kS2CReportWrongPwd=0x2;//登录失败，密码错误 
    static const unsigned short kS2CReportNoExist=0x3;//登录失败，用户名不存在 
    static const unsigned short kS2CReportMustUpdate=0x4;//登录失败，需要强制改密 
    static const unsigned short kS2CReportDuplicated=0x5;//注册失败，用户名已存在 
    static const unsigned short kS2CReportNameNotAccess=0x6;//注册失败，用户名不符合要求 
    static const unsigned short kS2CReportPwdNotAccess=0x7;//注册失败，密码不符合要求 
    static const unsigned short kS2CReportUpdateSucess=0x8;//更改密码成功 
    static const unsigned short kS2CReportLastPwdWrong=0x9;//更改密码失败，原密码错误 
    static const unsigned short kS2CReportNowPwdNotAccess=0xa;//更改密码失败，现密码不符合规范
    static const unsigned short kS2CInformOnline=0x0;//上线
    static const unsigned short kS2CInformOffline=0x1;//下线
    static const unsigned short kS2CTextSimpleText=0x0;//文本消息
    static const unsigned short kS2CTextFileInfo=0x1;//文件信息
    static const unsigned short kS2CTextFileContain=0x2;//文件内容
    static const unsigned short kS2CTextAskForClr=0x3;//清屏
    static const unsigned short kS2CUserSetUpdate=0x0;//发送用户设置
private:
    unsigned short packet_type;
    unsigned short function_type;
    unsigned int length;
};
#endif //MACRO
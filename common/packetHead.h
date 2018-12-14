#ifndef PACKETHEAD 
#define PACKETHEAD
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
#include<arpa/inet.h>
using namespace std;
/*为了方便统一，因而只有getString和toString才包括网络序和主机序的转化，其他均为正常用法*/
class packetHead{
    unsigned short packetType;
    unsigned short functionType;
    unsigned int length;
public:
    packetHead();
    packetHead(unsigned short,unsigned short,unsigned int);
    /*setString可以将网络序的规范字符串转为该类（主机序）内部存储形式*/
    /*getString可以将该类内部存储（主机序）改为网络序的规范字符串*/
    /*其他均为主机序的自定义设置和获取*/
    unsigned short getPacketType();
    unsigned short getFunctionType();
    unsigned short getLength();
    void setPacketType(unsigned short);
    void setFunctionType(unsigned short);
    void setLength(unsigned int);
    void getString(char*);
    void setString(const char*);
};
#endif //MACRO
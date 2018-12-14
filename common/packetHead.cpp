#include "packetHead.h"
packetHead::packetHead():packetType(0x00),functionType(0x00),length(0){}
packetHead::packetHead(unsigned short p,unsigned short f,unsigned int l):packetType(p),functionType(f),length(l){}
unsigned short packetHead::getPacketType()
{
    return packetType;
}
unsigned short packetHead::getFunctionType()
{
    return functionType;
}
unsigned short packetHead::getLength()
{
    return length;
}
void packetHead::setPacketType(unsigned short p)
{
    packetType=p;
}
void packetHead::setFunctionType(unsigned short f)
{
    functionType=f;
}
void packetHead::setLength(unsigned int l)
{
    length=l;
}
void packetHead::getString(char* s)
{
    unsigned short tmps;
    unsigned int tmpi;
    tmps=htons(packetType);
    memcpy(s, (char*)(&tmps) ,2);
    tmps=htons(functionType);
    memcpy(s+2, (char*)(&tmps),2);
    tmpi=htonl(length);
    memcpy(s+4, (char*)(&tmpi),4);
    return;
}
void packetHead::setString(const char* s)
{
    packetType=ntohs((*((unsigned short*)s))); 
    functionType=ntohs((*((unsigned short*)(s+2))));
    length=ntohl((*((unsigned int*)(s+4)))); 
    return;   
}
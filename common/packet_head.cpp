//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is packet head.
#include "packet_head.h"
PacketHead::PacketHead():packet_type(0),function_type(0),length(0){}
PacketHead::PacketHead(const unsigned short p,const unsigned short f,const unsigned int l)
{
    packet_type=p;
    function_type=f;
    length=l;
}
unsigned short PacketHead::get_packet_type() 
{
    return packet_type;
}
unsigned short PacketHead::get_function_type() 
{
    return function_type;
}
unsigned int PacketHead::get_length() const
{
    return length;
}
void PacketHead::get_string(char* s)
{
    unsigned short tmps_1=htons(packet_type);
    unsigned short tmps_2=htons(function_type);
    unsigned int tmpi=htonl(length);
    memcpy(s, (char*)(&tmps_1) ,2);
    memcpy(s+2, (char*)(&tmps_2),2);
    memcpy(s+4, (char*)(&tmpi),4);
    return;
}
void PacketHead::set_packet_type(const unsigned short p)
{
    packet_type=p;
    return;
}
void PacketHead::set_function_type(const unsigned short f)
{
    function_type=f;
    return;
}
void PacketHead::set_length(const unsigned int l)
{
    length=l;
    return;
}
void PacketHead::set_string(const char* s)
{
    packet_type=ntohs((*((unsigned short*)s))); 
    function_type=ntohs((*((unsigned short*)(s+2))));
    length=ntohl((*((unsigned int*)(s+4)))); 
    return;   
}
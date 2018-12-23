//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is packet server send to client.
#include "server_to_client.h"
/*ServerToClientBase*/
ServerToClientBase::ServerToClientBase():my_head(){}
ServerToClientBase::ServerToClientBase(const PacketHead& ph)
{
    my_head=ph;
}
void ServerToClientBase::get_string(char* s)
{
    my_head.get_string(s);
}
PacketHead ServerToClientBase::get_packet_head()
{
    return my_head;
}
void ServerToClientBase::set_string(const PacketHead& ph,const char* s)
{
    my_head=ph;
}
/*ServerToClientReportSuccess*/
ServerToClientReportSuccess::ServerToClientReportSuccess():ServerToClientBase()
{
    user_status=NULL;
}
ServerToClientReportSuccess::ServerToClientReportSuccess(const PacketHead& ph,const char*ltime,const int unum,char**ustatus):ServerToClientBase(ph)
{
    memcpy(last_login_time,ltime,19);
    last_login_time[19]=0;
    user_num=unum;
    if(!unum)
        return;
    user_status=new char*[user_num];
    for(int i=0;i<user_num;i++){
        user_status[i]=new char[33];
        memcpy(user_status[i],ustatus[i],33);
    }
}
ServerToClientReportSuccess::~ServerToClientReportSuccess()
{
    for(int i=0;i<user_num;i++){
        delete[] user_status[i];
    }
    delete[] user_status;
}
void ServerToClientReportSuccess::get_string(char* s)
{
    int tmpi=htonl(user_num);
    ServerToClientBase::get_string(s);
    //printf("%s\n", last_login_time);
    memcpy(s+8,last_login_time,19);
    //printf("%d\n", tmpi);
    memcpy(s+27,(char*)(&tmpi),4);
    for (int i = 0; i < user_num; i++)
    {
        memcpy(s + 31 + i * 33, user_status[i], 33);
    }
}
char* ServerToClientReportSuccess::get_last_login_time()
{
    return last_login_time;
}
unsigned int ServerToClientReportSuccess::get_user_num()
{
    return user_num;
}
char** ServerToClientReportSuccess::get_user_status()
{
    return user_status;
}
void ServerToClientReportSuccess::set_string(const PacketHead& ph,const char* s)
{
    if(user_status!=NULL)
    {
        for(int i=0;i<user_num;i++){
            delete[] user_status[i];
        }
        delete[] user_status;
    }
    ServerToClientBase::set_string(ph,s);
    memcpy(last_login_time,s,19);
    last_login_time[19]=0;
    user_num=ntohl((*((unsigned int*)(s+19)))); 
    user_status=new char*[user_num];
    for(int i=0;i<user_num;i++){
        user_status[i]=new char[33];
        memcpy(user_status[i],s+23+33*i,33);
    }
} 
/*ServerToClientInform*/ 
ServerToClientInform::ServerToClientInform():ServerToClientBase(){}
ServerToClientInform::ServerToClientInform(const PacketHead& ph,const char*uname):ServerToClientBase(ph)
{
    strcpy(user_name,uname);
    user_name[32]=0;
}
void ServerToClientInform::get_string(char* s)
{
    ServerToClientBase::get_string(s);
    memcpy(s+8,user_name,32);
}
char* ServerToClientInform::get_user_name()
{
    return user_name;
}
void ServerToClientInform::set_string(const PacketHead& ph,const char* s)
{
    ServerToClientBase::set_string(ph,s);
    strcpy(user_name,s);
    user_name[32]=0;
}  
/*ServerToClientText*/
ServerToClientText::ServerToClientText():ServerToClientBase(){}
ServerToClientText::ServerToClientText(const PacketHead& ph,const char*frname,const char* toname,const char*ntime):ServerToClientBase(ph)
{
    strcpy(user_from_name,frname);
    strcpy(user_to_name,toname);
    memcpy(now_time,ntime,19);
    now_time[19]=0;
}
void ServerToClientText::get_string(char* s)
{
    ServerToClientBase::get_string(s);
    memcpy(s+8,user_from_name,32);
    memcpy(s+40,user_to_name,32);
    memcpy(s+72,now_time,19);
}
char* ServerToClientText::get_user_from_name()
{
    return user_from_name;
}
char* ServerToClientText::get_user_to_name()
{
    return user_to_name;
}
char* ServerToClientText::get_now_time()
{
    return now_time;
}
void ServerToClientText::set_string(const PacketHead& ph,const char* s)
{
    ServerToClientBase::set_string(ph,s);
    strcpy(user_from_name,s);
    user_from_name[32]=0;  
    strcpy(user_to_name,s+32);
    memcpy(now_time,s+64,19);
    now_time[19]=0;
}  
/*ServerToClientTextSimpleText*/
ServerToClientTextSimpleText::ServerToClientTextSimpleText():ServerToClientText()
{
    simple_text_contain=NULL;
}
ServerToClientTextSimpleText::ServerToClientTextSimpleText(const PacketHead& ph,const char*frname,const char* toname,const char*ntime,const char*scontain):ServerToClientText(ph,frname,toname,ntime)
{
    text_length=ph.get_length()-32-19-32;
    simple_text_contain=new char[text_length+1];
    memcpy(simple_text_contain,scontain,text_length);
    simple_text_contain[text_length]=0;
}
ServerToClientTextSimpleText::~ServerToClientTextSimpleText()
{
    delete[] simple_text_contain;
}
void ServerToClientTextSimpleText::get_string(char* s)
{
    ServerToClientText::get_string(s);
    memcpy(s+8+32+19+32,simple_text_contain,text_length);
}
char* ServerToClientTextSimpleText::get_simple_text_contain()
{
    return simple_text_contain;
}
int ServerToClientTextSimpleText::get_text_length()
{
    return text_length;
}
void ServerToClientTextSimpleText::set_string(const PacketHead& ph,const char* s)
{
    if(simple_text_contain!=NULL)
        delete[] simple_text_contain;
    ServerToClientText::set_string(ph,s);
    text_length=ph.get_length()-32-19-32;
    simple_text_contain=new char[text_length+1];
    memcpy(simple_text_contain,s+19+32+32,text_length);
    simple_text_contain[text_length]=0;   
}  
/*ServerToClientTextFileInfo*/
ServerToClientTextFileInfo::ServerToClientTextFileInfo():ServerToClientText(){}
ServerToClientTextFileInfo::ServerToClientTextFileInfo(const PacketHead& ph,const char*frname,const char* toname,const char*ntime,const char*fname,const unsigned int fkey):ServerToClientText(ph,frname,toname,ntime)
{
    strcpy(file_name,fname);
    file_name[64]=0;
    file_key=fkey;
}
void ServerToClientTextFileInfo::get_string(char* s)
{
    int tmpi=htonl(file_key);
    ServerToClientText::get_string(s);
    memcpy(s+8+32+19+32,file_name,64);
    memcpy(s+8+32+19+64+32,(char*)(&tmpi),4);   
}
char* ServerToClientTextFileInfo::get_file_name()
{
    return file_name;
}
unsigned int ServerToClientTextFileInfo::get_file_key()
{
    return file_key;
}
void ServerToClientTextFileInfo::set_string(const PacketHead& ph,const char* s)
{
    ServerToClientText::set_string(ph,s);
    strcpy(file_name,s+19+32+32);
    file_name[64]=0; 
    file_key=ntohl((*((unsigned int*)(s+19+32+64+32)))); 
} 
/*ServerToClientTextFileContain*/
ServerToClientTextFileContain::ServerToClientTextFileContain():ServerToClientTextFileInfo()
{
    file_contain=NULL;
}
ServerToClientTextFileContain::ServerToClientTextFileContain(const PacketHead& ph,const char*frname,const char* toname,const char*ntime,const char*fname,const unsigned int fkey,const char* fcontain):
                                ServerToClientTextFileInfo(ph,frname,toname,ntime,fname,fkey)
{
    text_length=ph.get_length()-32-19-64-4-32;
    file_contain=new char[text_length+1];
    memcpy(file_contain,fcontain,text_length);
    file_contain[text_length]=0;
}
ServerToClientTextFileContain::~ServerToClientTextFileContain()
{
    delete[] file_contain;
    //printf("delete ok\n");
}
void ServerToClientTextFileContain::get_string(char* s)
{
    ServerToClientTextFileInfo::get_string(s);
    memcpy(s+8+32+19+64+4+32,file_contain,text_length);
}
char* ServerToClientTextFileContain::get_file_contain()
{
    return file_contain;
}
int ServerToClientTextFileContain::get_text_length()
{
    return text_length;
}
void ServerToClientTextFileContain::set_string(const PacketHead& ph,const char* s)
{
    if(file_contain!=NULL)
        delete[] file_contain;
    text_length=ph.get_length()-32-19-64-4-32;
    ServerToClientTextFileInfo::set_string(ph,s);
    memcpy(file_contain,s+32+19+64+4+32,text_length);
    file_contain[text_length]=0;
}  
/*ServerToClientUserSetUpdate*/
ServerToClientUserSetUpdate::ServerToClientUserSetUpdate():ServerToClientBase()
{
    user_set_data=NULL;
}
ServerToClientUserSetUpdate::ServerToClientUserSetUpdate(const PacketHead& ph,const char*sc):ServerToClientBase(ph)
{
    int ulength=ph.get_length();
    user_set_data=new char[ulength+1];
    memcpy(user_set_data,sc,ulength);
    user_set_data[ulength]=0;
}
ServerToClientUserSetUpdate::~ServerToClientUserSetUpdate()
{
    delete[] user_set_data;
}
void ServerToClientUserSetUpdate::get_string(char* s)
{
    ServerToClientBase::get_string(s);
    memcpy(s+8,user_set_data,get_packet_head().get_length());
}
char* ServerToClientUserSetUpdate::get_user_set_data()
{
    return user_set_data;
}
int ServerToClientUserSetUpdate::get_text_length()
{
    return get_packet_head().get_length();
}
void ServerToClientUserSetUpdate::set_string(const PacketHead& ph,const char* s)
{
    if(user_set_data!=NULL)
        delete[] user_set_data;
    ServerToClientBase::set_string(ph,s);
    int ulength=ph.get_length();
    user_set_data=new char[ulength+1];
    memcpy(user_set_data,s,ulength);
    user_set_data[ulength]=0;   
} 
//TODO

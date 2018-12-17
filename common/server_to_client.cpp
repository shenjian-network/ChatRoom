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
ServerToClientReportSuccess::ServerToClientReportSuccess():ServerToClientBase(){}
ServerToClientReportSuccess::ServerToClientReportSuccess(const PacketHead& ph,char*ltime,int unum,char**ustatus):ServerToClientBase(ph)
{
    memcpy(last_login_time,ltime,19);
    last_login_time[19]=0;
    user_num=unum;
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
    memcpy(s+8,last_login_time,19);
    memcpy(s+27,(char*)(&tmpi),4);
    for(int i=0;i<user_num;i++){
        memcpy(s+31+i*33,user_status[i],33); 
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
    ServerToClientBase::set_string(ph,s);
    memcpy(last_login_time,s,19);
    last_login_time[19]=0;
    user_num=ntohs((*((unsigned int*)(s+19)))); 
    user_status=new char*[user_num];
    for(int i=0;i<user_num;i++){
        user_status[i]=new char[33];
        memcpy(user_status[i],s+23+33*i,33);
    }
} 
/*ServerToClientInform*/ 
ServerToClientInform::ServerToClientInform():ServerToClientBase(){}
ServerToClientInform::ServerToClientInform(const PacketHead& ph,char*uname):ServerToClientBase(ph)
{
    memcpy(user_name,uname,32);
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
    memcpy(user_name,s,32);
    user_name[32]=0;
}  
/*ServerToClientText*/
ServerToClientText::ServerToClientText():ServerToClientBase(){}
ServerToClientText::ServerToClientText(const PacketHead& ph,char*uname,char*ntime):ServerToClientBase(ph)
{
    memcpy(user_from_name,uname,32);
    memcpy(now_time,ntime,19);
    user_from_name[32]=0;
    now_time[19]=0;
}
void ServerToClientText::get_string(char* s)
{
    ServerToClientBase::get_string(s);
    memcpy(s+8,user_from_name,32);
    memcpy(s+40,now_time,19);
}
char* ServerToClientText::get_user_from_name()
{
    return user_from_name;
}
char* ServerToClientText::get_now_time()
{
    return now_time;
}
void ServerToClientText::set_string(const PacketHead& ph,const char* s)
{
    ServerToClientBase::set_string(ph,s);
    memcpy(user_from_name,s,32);
    user_from_name[32]=0;  
    memcpy(now_time,s+32,19);
    now_time[19]=0;
}  
/*ServerToClientTextSimpleText*/
ServerToClientTextSimpleText::ServerToClientTextSimpleText():ServerToClientText(){}
ServerToClientTextSimpleText::ServerToClientTextSimpleText(const PacketHead& ph,char*uname,char*ntime,char*scontain):ServerToClientText(ph,uname,ntime)
{
    text_length=ph.get_length()-32-19;
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
    memcpy(s+8+32+19,simple_text_contain,text_length);
}
char* ServerToClientTextSimpleText::get_simple_text_contain()
{
    return simple_text_contain;
}
void ServerToClientTextSimpleText::set_string(const PacketHead& ph,const char* s)
{
    ServerToClientText::set_string(ph,s);
    text_length=ph.get_length()-32-19;
    simple_text_contain=new char[text_length+1];
    memcpy(simple_text_contain,s+19+32,text_length);
    simple_text_contain[text_length]=0;   
}  
/*ServerToClientTextFileInfo*/
ServerToClientTextFileInfo::ServerToClientTextFileInfo():ServerToClientText(){}
ServerToClientTextFileInfo::ServerToClientTextFileInfo(const PacketHead& ph,char*uname,char*ntime,char*fname,unsigned int fkey):ServerToClientText(ph,uname,ntime)
{
    memcpy(file_name,fname,64);
    file_name[64]=0;
    file_key=fkey;
}
void ServerToClientTextFileInfo::get_string(char* s)
{
    int tmpi=htonl(file_key);
    ServerToClientText::get_string(s);
    memcpy(s+8+32+19,file_name,64);
    memcpy(s+8+32+19+64,(char*)(&tmpi),4);   
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
    memcpy(file_name,s+19+32,64);
    file_name[64]=0; 
    file_key=ntohs((*((unsigned int*)(s+19+32+64)))); 
} 
/*ServerToClientTextFileContain*/
ServerToClientTextFileContain::ServerToClientTextFileContain():ServerToClientTextFileInfo(){}
ServerToClientTextFileContain::ServerToClientTextFileContain(const PacketHead& ph,char*uname,char*ntime,char*fname,unsigned int fkey,char* fcontain):
                                ServerToClientTextFileInfo(ph,uname,ntime,fname,fkey)
{
    file_length=ph.get_length()-32-19-64-4;
    file_contain=new char[file_length+1];
    memcpy(file_contain,fcontain,file_length);
    file_contain[file_length]=0;
}
ServerToClientTextFileContain::~ServerToClientTextFileContain()
{
    delete[] file_contain;
}
void ServerToClientTextFileContain::get_string(char* s)
{
    ServerToClientTextFileInfo::get_string(s);
    memcpy(s+8+32+19+64+4,file_contain,file_length);
}
char* ServerToClientTextFileContain::get_file_contain()
{
    return file_contain;
}
void ServerToClientTextFileContain::set_string(const PacketHead& ph,const char* s)
{
    file_length=ph.get_length()-32-19-64-4;
    ServerToClientTextFileInfo::set_string(ph,s);
    memcpy(file_contain,s+32+19+64+4,file_length);
    file_contain[file_length]=0;
}  
/*ServerToClientUserSetUpdate*/
ServerToClientUserSetUpdate::ServerToClientUserSetUpdate():ServerToClientBase(){}
ServerToClientUserSetUpdate::ServerToClientUserSetUpdate(const PacketHead& ph,char*sc):ServerToClientBase(ph)
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
void ServerToClientUserSetUpdate::set_string(const PacketHead& ph,const char* s)
{
    ServerToClientBase::set_string(ph,s);
    int ulength=ph.get_length();
    user_set_data=new char[ulength+1];
    memcpy(user_set_data,s+8,ulength);
    user_set_data[ulength]=0;   
} 
//TODO
int main()
{
    printf("test\n");
}
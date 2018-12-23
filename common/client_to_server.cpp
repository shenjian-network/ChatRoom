//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is packet client send to server.
#include "client_to_server.h"
/*ClientToServerBase*/
ClientToServerBase::ClientToServerBase():my_head(){}
ClientToServerBase::ClientToServerBase(const PacketHead& ph)
{
    my_head=ph;
}
void ClientToServerBase::get_string(char* s)
{
    my_head.get_string(s);
}
PacketHead ClientToServerBase::get_packet_head()
{
    return my_head;
}
void ClientToServerBase::set_string(const PacketHead& ph,const char* s)
{
    my_head=ph;
}
/*ClientToServerReportLogin*/
ClientToServerReportLogin::ClientToServerReportLogin():ClientToServerBase(){}
ClientToServerReportLogin::ClientToServerReportLogin(const PacketHead& ph,const char* uname,const char* upwd):ClientToServerBase(ph)
{
    strcpy(user_name,uname);
    strcpy(user_pwd,upwd);
    user_name[32]=0;
    user_pwd[32]=0;
}
void ClientToServerReportLogin::get_string(char* s)
{
    ClientToServerBase::get_string(s);
    memcpy(s+8,user_name,32);
    memcpy(s+40,user_pwd,32);
}
char* ClientToServerReportLogin::get_user_name()
{
    return user_name;
}
char* ClientToServerReportLogin::get_user_pwd()
{
    return user_pwd;
}
void ClientToServerReportLogin::set_string(const PacketHead& ph,const char* s)
{
    ClientToServerBase::set_string(ph,s);
    strcpy(user_name,s);
    strcpy(user_pwd,s+32);
    user_name[32]=0;
    user_pwd[32]=0;
}
/*ClientToServerReportUpdate*/
ClientToServerReportUpdate::ClientToServerReportUpdate():ClientToServerBase(){}
ClientToServerReportUpdate::ClientToServerReportUpdate(const PacketHead& ph,const char* uname,const char* last_upwd,const char* now_upwd):ClientToServerBase(ph)
{
    strcpy(user_name,uname);
    strcpy(last_user_pwd,last_upwd);
    strcpy(now_user_pwd,now_upwd);
    user_name[32]=0;
    last_user_pwd[32]=0;
    now_user_pwd[32]=0;
}
void ClientToServerReportUpdate::get_string(char* s)
{
    ClientToServerBase::get_string(s);
    memcpy(s+8,user_name,32);
    memcpy(s+40,last_user_pwd,32);  
    memcpy(s+72,now_user_pwd,32);    
}
char* ClientToServerReportUpdate::get_user_name()
{
    return user_name;
}
char* ClientToServerReportUpdate::get_last_user_pwd()
{
    return last_user_pwd;
}
char* ClientToServerReportUpdate::get_now_user_pwd()
{
    return now_user_pwd;
}
void ClientToServerReportUpdate::set_string(const PacketHead& ph,const char* s)
{
    ClientToServerBase::set_string(ph,s);
    strcpy(user_name,s);
    strcpy(last_user_pwd,s+32);
    strcpy(now_user_pwd,s+64);
    user_name[32]=0;
    last_user_pwd[32]=0;   
    now_user_pwd[32]=0;
}  
/*ClientToServerTextToUsers*/
ClientToServerTextToUsers::ClientToServerTextToUsers():ClientToServerBase()
{
    user_info=NULL;
    text_info=NULL;
}
ClientToServerTextToUsers::ClientToServerTextToUsers(const PacketHead& ph,const int& unum,char** uinfo,const char* cinfo):ClientToServerBase(ph)
{
    text_length=ph.get_length()-unum*32-4;
    user_num=unum;
    user_info=new char*[user_num];
    for(int i=0;i<user_num;i++){
        user_info[i]=new char[33];
        strcpy(user_info[i],uinfo[i]);
        user_info[i][32]=0;
    }
    text_info=new char[text_length+1];
    memcpy( text_info, cinfo,text_length);
    text_info[text_length]=0;
}
ClientToServerTextToUsers::~ClientToServerTextToUsers()
{
    for(int i=0;i<user_num;i++){
        delete[] user_info[i];
    }
    delete[] user_info;
    delete[] text_info; 
}
void ClientToServerTextToUsers::get_string(char* s)
{
    int tmpi=htonl(user_num);
    ClientToServerBase::get_string(s);
    memcpy(s+8,(char*)(&tmpi),4);
    for(int i=0;i<user_num;i++){
        memcpy(s+12+i*32,user_info[i],32); 
    }
    memcpy(s+12+user_num*32,text_info,text_length);    
}
unsigned int ClientToServerTextToUsers::get_user_num()
{
    return user_num;
}
char** ClientToServerTextToUsers::get_user_info()
{
    return user_info;
}
char* ClientToServerTextToUsers::get_text_info()
{
    return text_info;
}
void ClientToServerTextToUsers::set_string(const PacketHead& ph,const char* s)
{
    if(user_info!=NULL){
        for(int i=0;i<user_num;i++){
            delete[] user_info[i];
        }
        delete[] user_info;
    }
    if(text_info!=NULL)
        delete[] text_info;
    ClientToServerBase::set_string(ph,s);
    user_num=ntohl((*((unsigned int*)s))); 
    text_length=ph.get_length()-user_num*32-4;
    user_info=new char*[user_num];
    for(int i=0;i<user_num;i++){
        user_info[i]=new char[33];
        strcpy(user_info[i],s+4+32*i);
        user_info[i][32]=0;
    }
    text_info=new char[text_length+1];
    memcpy( text_info,s+4+32*user_num,text_length);
    text_info[text_length]=0;        
}  
int ClientToServerTextToUsers::get_text_length()
{
    return text_length;
}
/*ClientToServerTextFileToUsers*/
ClientToServerTextFileToUsers::ClientToServerTextFileToUsers():ClientToServerBase()
{
    user_info=NULL;
    text_info=NULL;
}
ClientToServerTextFileToUsers::ClientToServerTextFileToUsers(const PacketHead& ph,const int& unum,char** uinfo,const char* cinfo,const char* fname):ClientToServerBase(ph)
{
    text_length=ph.get_length()-unum*32-4-64;
    user_num=unum;
    user_info=new char*[user_num];
    for(int i=0;i<user_num;i++){
        user_info[i]=new char[33];
        strcpy(user_info[i],uinfo[i]);
        user_info[i][32]=0;
    }
    strcpy(file_name,fname);
    file_name[64]=0;
    text_info=new char[text_length+1];
    memcpy( text_info, cinfo,text_length);
    text_info[text_length]=0;
}
ClientToServerTextFileToUsers::~ClientToServerTextFileToUsers()
{
    for(int i=0;i<user_num;i++){
        delete[] user_info[i];
    }
    delete[] user_info;
    delete[] text_info;    
}
void ClientToServerTextFileToUsers::get_string(char* s)
{
    int tmpi=htonl(user_num);
    ClientToServerBase::get_string(s);
    memcpy(s+8,(char*)(&tmpi),4);
    for(int i=0;i<user_num;i++){
        memcpy(s+12+i*32,user_info[i],32); 
    }
    memcpy(s+12+user_num*32,file_name,64);
    memcpy(s+12+user_num*32+64,text_info,text_length);    
}
unsigned int ClientToServerTextFileToUsers::get_user_num()
{
    return user_num;
}
char** ClientToServerTextFileToUsers::get_user_info()
{
    return user_info;
}
char* ClientToServerTextFileToUsers::get_text_info()
{
    return text_info;
}
char* ClientToServerTextFileToUsers::get_file_name()
{
    return file_name;
}
int ClientToServerTextFileToUsers::get_text_length()
{
    return text_length;
}
void ClientToServerTextFileToUsers::set_string(const PacketHead& ph,const char* s)
{
    if(user_info!=NULL){
        for(int i=0;i<user_num;i++){
            delete[] user_info[i];
        }
        delete[] user_info;
    }
    if(text_info!=NULL)
        delete[] text_info;
    ClientToServerBase::set_string(ph,s);
    user_num=ntohl((*((unsigned int*)s))); 
    text_length=ph.get_length()-user_num*32-4-64;
    user_info=new char*[user_num];
    for(int i=0;i<user_num;i++){
        user_info[i]=new char[33];
        strcpy(user_info[i],s+4+32*i);
        user_info[i][32]=0;
    }
    strcpy(file_name,s+4+32*user_num);
    file_name[64]=0;
    text_info=new char[text_length+1];
    memcpy(text_info,s+4+32*user_num+64,text_length);
    text_info[text_length]=0;     
}
/*ClientToServerTextAskForFile*/
ClientToServerTextAskForFile::ClientToServerTextAskForFile():ClientToServerBase(){}
ClientToServerTextAskForFile::ClientToServerTextAskForFile(const PacketHead& ph,const int& fkey):ClientToServerBase(ph)
{
    file_key=fkey;
}
void ClientToServerTextAskForFile::get_string(char* s)
{
    int tmpi=htonl(file_key);
    ClientToServerBase::get_string(s);
    memcpy(s+8,(char*)(&tmpi),4);    
}
unsigned int ClientToServerTextAskForFile::get_file_key()
{
    return file_key;
}
void ClientToServerTextAskForFile::set_string(const PacketHead& ph,const char* s)
{
    ClientToServerBase::set_string(ph,s);
    file_key=ntohl((*((unsigned int*)s))); 
} 
/*ClientToServerTextAskForTexts*/
ClientToServerTextAskForTexts::ClientToServerTextAskForTexts():ClientToServerBase(){}
ClientToServerTextAskForTexts::ClientToServerTextAskForTexts(const PacketHead& ph,const int& lnum,const char* ouser):ClientToServerBase(ph)
{
    list_num=lnum;
    strcpy(op_user,ouser);
}
void ClientToServerTextAskForTexts::get_string(char* s)
{
    int tmpi=htonl(list_num);
    ClientToServerBase::get_string(s);
    memcpy(s+8,(char*)(&tmpi),4);    
    memcpy(s+12,op_user,32);   
}
unsigned int ClientToServerTextAskForTexts::get_list_num()
{
    return list_num;
}
char* ClientToServerTextAskForTexts::get_op_user()
{
    return op_user;
}
void ClientToServerTextAskForTexts::set_string(const PacketHead& ph,const char* s)
{
    ClientToServerBase::set_string(ph,s);
    list_num=ntohl((*((unsigned int*)s)));   
    strcpy(op_user,(s+4)); 
} 
/*ClientToServerUserSetUpdate*/
ClientToServerUserSetUpdate::ClientToServerUserSetUpdate():ClientToServerBase()
{
    user_set_data=NULL;
}
ClientToServerUserSetUpdate::ClientToServerUserSetUpdate(const PacketHead& ph,const char* sc):ClientToServerBase(ph)
{
    user_set_data=new char[ph.get_length()+1];
    memcpy(user_set_data,sc,ph.get_length());
    user_set_data[ph.get_length()]=0;
}
ClientToServerUserSetUpdate::~ClientToServerUserSetUpdate()
{
    delete[] user_set_data;
}
void ClientToServerUserSetUpdate::get_string(char* s)
{
    ClientToServerBase::get_string(s);
    memcpy(s+8,user_set_data,get_packet_head().get_length());      
}
char* ClientToServerUserSetUpdate::get_user_set_data()
{
    return user_set_data;
}
int ClientToServerUserSetUpdate::get_text_length()
{
    return get_packet_head().get_length();
}
void ClientToServerUserSetUpdate::set_string(const PacketHead& ph,const char* s)
{
    if(user_set_data!=NULL)
        delete[] user_set_data;
    ClientToServerBase::set_string(ph,s);
    user_set_data=new char[ph.get_length()+1];
    memcpy(user_set_data,s,ph.get_length()); 
    user_set_data[ph.get_length()]=0; 
}  
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~分割分割~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*SenderToReceiverFileNotify*/
SenderToReceiverFileNotify::SenderToReceiverFileNotify():ClientToServerBase(){}
SenderToReceiverFileNotify::SenderToReceiverFileNotify(const PacketHead& ph,const char* sn,const char* rn,const char* fn,unsigned int fs):ClientToServerBase(ph)
{
    strcpy(sender_name,sn);
    strcpy(receiver_name,rn);
    strcpy(file_name,fn);
    file_size=fs;
}
void SenderToReceiverFileNotify::get_string(char* s)
{
    int tmpi=htonl(file_size);
    ClientToServerBase::get_string(s);
    memcpy(s+8,sender_name,32);
    memcpy(s+40,receiver_name,32);
    memcpy(s+72,file_name,64);
    memcpy(s+72+64,(char*)(&tmpi),4);        
}
char* SenderToReceiverFileNotify::get_sender_name()
{
    return sender_name;
}
char* SenderToReceiverFileNotify::get_receiver_name()
{
    return receiver_name;
}
char* SenderToReceiverFileNotify::get_file_name()
{
    return file_name;
}
unsigned int SenderToReceiverFileNotify::get_file_size()
{
    return file_size;
}
unsigned int SenderToReceiverFileNotify::get_block_num()
{
    return file_size;
}
void SenderToReceiverFileNotify::set_string(const PacketHead& ph,const char* s)
{
    ClientToServerBase::set_string(ph,s);
    strcpy(sender_name,s);
    strcpy(receiver_name,s+32);
    strcpy(file_name,s+64);
    file_size=ntohl((*((unsigned int*)(s+128))));
}
/*SenderToReceiverFileData*/
SenderToReceiverFileData::SenderToReceiverFileData():SenderToReceiverFileNotify(){}
SenderToReceiverFileData::SenderToReceiverFileData(const PacketHead& ph,const char* sn,const char* rn,const char* fn,unsigned int bn,const char* fc): \
                                                SenderToReceiverFileNotify(ph,sn,rn,fn,bn)
{
    memcpy(file_contain,fc,FILEBUFFERSIZE);
}
char* SenderToReceiverFileData::get_file_contain()
{
    return file_contain;
}
void SenderToReceiverFileData::get_string(char* s)
{
    SenderToReceiverFileNotify::get_string(s);
    memcpy(s+140,file_contain,FILEBUFFERSIZE);
}
void SenderToReceiverFileData::set_string(const PacketHead& ph,const char* s)
{
    SenderToReceiverFileNotify::set_string(ph,s);
    memcpy(file_contain,s+132,FILEBUFFERSIZE);
}

//TODO
// int main()
// {     //for test
//      printf("test:\n");
//      PacketHead ph;
//      ClientToServerBase* cb=new ClientToServerUserSetUpdate(ph,"123");
//      delete cb;
//      return 1;
// }

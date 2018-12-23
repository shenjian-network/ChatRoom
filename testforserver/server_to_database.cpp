//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is a database owned by server.
#include "server_to_database.h"
ServerToDatabase::ServerToDatabase():mydb()
{
    my_result=NULL;
    my_lengths=NULL;
    last_new_line=0;
    last_new_row=0;
}
char*** ServerToDatabase::get_my_result()
{
    return my_result;
}
unsigned long** ServerToDatabase::get_my_lengths()
{
    return my_lengths;
}
void ServerToDatabase::NewMultiArray(const int row,const int line,const int general_size,const int last_size)
{
    DeleteMultiArray();
    last_new_row=row;
    last_new_line=line;
    my_result=new char**[row];
    my_lengths=new unsigned long*[row];
    for(int i=0;i<row;i++){
        my_result[i]=new char*[line];
        my_lengths[i]=new unsigned long[line];
        for(int j=0;j<line;j++){
            if(j!=line-1){
                my_result[i][j]=new char[general_size];
            }else{
                my_result[i][j]=new char[last_size];
            }
        }
    }
}
void ServerToDatabase::DeleteMultiArray()
{
    if(my_result==NULL)
        return;
    for(int i=0;i<last_new_row;i++){
        for(int j=0;j<last_new_line;j++){
            delete[] my_result[i][j];
        }
        delete[] my_result[i];
        delete[] my_lengths[i];
    }
    delete[] my_result;
    delete my_lengths;
}
int ServerToDatabase::GetUserInfo()
{
    int ret;
    mydb.get_conn();
    NewMultiArray(10000,1 ,50 ,50);
    ret=mydb.MySelect(10000,1,"select `uname` from `user_info`",my_result,my_lengths);
    mydb.close_conn();
    return ret;
}
bool ServerToDatabase::CheckUserLogin(const char* name,const char* passwd)
{
    int ret;
    mydb.get_conn();
    NewMultiArray(1 , 2 ,50 ,3*1024*1024);
    char format_q[255]="select `last_login_time`,`user_set` from `user_info` where `uname`='%s' and `pwd`='%s'";
    char my_q[255];
    snprintf(my_q,255 ,format_q,name,passwd);
    ret=mydb.MySelect(1,2,my_q,my_result,my_lengths);
    mydb.close_conn();  
    return ret;
}
void ServerToDatabase::UpdateLastLoginTime(const char* name,const char* now_time)
{
    int ret;
    mydb.get_conn();
    //NewMultiArray(1 , 2 ,50 ,3*1024*1024);
    char format_q[255]="update `user_info` set `last_login_time`='%s' where `uname`='%s'";
    char my_q[255];
    snprintf(my_q,255,format_q,now_time,name);
    ret=mydb.MyNoSelect(my_q);
    mydb.close_conn();  
    return;
}
void ServerToDatabase::AddUser(const char* name,const char* passwd,const char* last_login_time)
{
    int ret;
    MYSQL * conn;
    char* default_set=new char[1024*1024];
    char* chunk_set=new char[2*1024*1024+1];
    strcpy(default_set,"NULL");
    int set_size=4;
    int query_len;
    conn=mydb.get_conn();
    mysql_real_escape_string(conn, chunk_set, default_set, set_size);
    char format_q[1024]="insert into `user_info` values('%s','%s','%s','%s')";
    char* my_q=new char[3*1024*1024];
    query_len=snprintf(my_q,3*1024*1024,format_q,name,passwd,last_login_time,chunk_set);
    ret=mydb.MyNoSelect(query_len,my_q);
    strcpy(format_q,"create table %s_msgtable("\
                "um_id int unsigned primary key auto_increment,"\
                "send_time datetime not null,"\
                "from_user char(32) not null," \
                "cid int unsigned," \
                "ctype char(1) default 'm'," \
                "foreign key(`from_user`) references user_info(`uname`)" \
            ")engine=innodb,charset=utf8,collate=utf8_bin");
    snprintf(my_q,3*1024*1024,format_q,name);
    ret=mydb.MyNoSelect(my_q);
    mydb.close_conn();
    delete[] default_set;
    delete[] chunk_set;
    delete[] my_q;  
    return; 
}
bool ServerToDatabase::UpdateUserPwd(const char* name,const char* last_pwd,const char* now_pwd)
{
    if(!CheckUserLogin(name,last_pwd)){
        return false;
    }
    int ret;
    mydb.get_conn();
    char format_q[255]="update `user_info` set `pwd`='%s' where `uname`='%s'";
    char my_q[255];
    snprintf(my_q,255,format_q,now_pwd,name);
    ret=mydb.MyNoSelect(my_q);
    mydb.close_conn();  
    return true;  
}
void ServerToDatabase::UpdateUserSet(const char* name,const char* u_set,const int text_length)
{
    int ret;
    MYSQL * conn;
    char* chunk_set=new char[2*1024*1024+1];
    int query_len;
    conn=mydb.get_conn();
    mysql_real_escape_string(conn, chunk_set, u_set, text_length);
    char format_q[255]="update `user_info` set `user_set`='%s' where `uname`='%s'";
    char* my_q=new char[3*1024*1024];
    query_len=snprintf(my_q,3*1024*1024,format_q,chunk_set,name);
    ret=mydb.MyNoSelect(query_len,my_q);
    mydb.close_conn(); 
    delete[] chunk_set;
    delete[] my_q;
}
unsigned int ServerToDatabase::PushBackTextContain(const char* tcontain)
{
    int ret;
    MYSQL * conn;
    conn=mydb.get_conn();
    char format_q[255]="insert into `text_contain` values(NULL,'%s')";
    char* my_q=new char[3*1024*1024];
    snprintf(my_q,3*1024*1024,format_q,tcontain);
    ret=mydb.MyNoSelect(my_q);
    mydb.close_conn(); 
    delete[] my_q;
    return mydb.get_last_insert_id();   
}
unsigned int ServerToDatabase::PushBackFileContain(const int& fsize,const char* fname,const char* fcontain)
{
    int ret;
    MYSQL * conn;
    char* chunk_fcontain=new char[2*1024*1024+1];
    int query_len;
    conn=mydb.get_conn();
    mysql_real_escape_string(conn, chunk_fcontain, fcontain, fsize);
    printf("insert:%s\n",chunk_fcontain);
    char format_q[255]="insert into `file_contain` values(NULL, %d ,'%s','%s')";
    char* my_q=new char[3*1024*1024];
    query_len=snprintf(my_q,3*1024*1024,format_q,fsize,fname,chunk_fcontain);
    ret=mydb.MyNoSelect(query_len,my_q);
    mydb.close_conn(); 
    delete[] chunk_fcontain;
    delete[] my_q;
    return mydb.get_last_insert_id(); 
}
unsigned int ServerToDatabase::PushBackUserReceive(const char* to_name,const char* send_time,const char* from_name,const unsigned int& cid,const char& ctype)
{
    int ret;
    MYSQL * conn;
    conn=mydb.get_conn();
    char format_q[255]="insert into `%s_msgtable` values(NULL,'%s','%s','%u','%c')";
    char* my_q=new char[1024*1024];
    snprintf(my_q,1024*1024,format_q,to_name,send_time,from_name,cid,ctype);
    ret=mydb.MyNoSelect(my_q);
    mydb.close_conn(); 
    delete[] my_q;
    return mydb.get_last_insert_id();  
}
int ServerToDatabase::GetLastTexts(const int& row_num,const char* name)
{
    int ret;
    mydb.get_conn();
    NewMultiArray(row_num+1,6 ,128 ,1024*1024);
    char format_q[512]="select `send_time`,`from_user`,`ctype`,`fid`,`fname`,`mcontain` from %s_msgtable as ust join text_contain on text_contain.`mid`=ust.`cid` join file_contain on ust.`cid`= file_contain.`fid` " \
                        "order by `send_time` desc limit 0,row_num";
    char my_q[512];   
    snprintf(my_q,512,format_q,name);
    ret=mydb.MySelect(row_num,6,my_q,my_result,my_lengths);
    mydb.close_conn();
    return ret;  
}
void ServerToDatabase::GetFile(const unsigned int& file_key,const char* name)
{
    int ret;
    mydb.get_conn();
    NewMultiArray( 1 , 5 ,128, 2*1024*1024+1);
    char format_q[512]="select `fsize`,`fname`,`from_user`,`send_time`,`fcontain` from file_contain join %s_msgtable as umsg on umsg.`cid`=file_contain.`fid` where `fid`=%u limit 0,1";
    char my_q[512];   
    snprintf(my_q,512,format_q,name,file_key);
    ret=mydb.MySelect(1,5,my_q,my_result,my_lengths);
    mydb.close_conn();
    return;     
}
//TODO
// int main()
// {
//     printf("test:\n");
//     char*** res;
//     unsigned long** lengths;
//     ServerToDatabase* sd=new ServerToDatabase();
//     int ret;
//     ret=sd->CheckUserLogin("zyc","123456");
//     printf("ret:%d\n",ret);
//     res=sd->get_my_result();
//     lengths=sd->get_my_lengths();
//     printf("time:%s length:%d",res[0][0],lengths[0][0]);
//     ret=sd->CheckUserLogin("zyc","1234567");
//     printf("rets:%d\n",ret);
//     ret=sd->GetUserInfo();
//     printf("rets:%d\n",ret);
//     res=sd->get_my_result();
//     lengths=sd->get_my_lengths();
//     for(int i=0;i<ret;i++)
//     {
//         for(int j=0;j<1;j++)
//         {
//             if(lengths[i][j])
//                 printf("%s\t",res[i][j]);
//         }
//         printf("\n");
//     }
//     printf("**********************************\n");
//     char text[100];
//     memset(text,0,sizeof(text));
//     text[1]='a';
//     text[4]='b';
//     ret=sd->PushBackFileContain(20,"fortest.jpg",text);
//     printf("rets:%d\n",ret);
//     sd->GetFile(ret);
//     res=sd->get_my_result();
//     lengths=sd->get_my_lengths();
//     for(int i=0;i<1;i++)
//     {
//         for(int j=2;j<3;j++)
//         {
//             printf("lengths:%d\n",lengths[i][j]);
//             for(int k=0;k<lengths[i][j];k++)
//             {
//                 printf("%c",res[i][j]);
//             }
//         }
//         printf("\n");
//     }    
//     //ret=sd.CheckUserLogin("zyc","123456");
//     //printf("ret:%d\n",ret);
// }
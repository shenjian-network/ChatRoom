//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is a database owned by server.
#include "server_to_database.h"
#include "md5.h"
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
    delete []my_lengths;
}
int ServerToDatabase::GetUserInfo()
{
    int ret;
    mydb.get_conn();
    NewMultiArray(1000,1 ,50 ,50);
    //printf("get user info\n");
    ret=mydb.MySelect(10000,1,"select `uname` from `user_info`",my_result,my_lengths);
    mydb.close_conn();
    xl.appendlog_sql_select("user_info", "select all user name", ret);

    return ret;
}
bool ServerToDatabase::CheckUserLogin(const char* name,const char* passwd)
{
    int ret;
    mydb.get_conn();
    NewMultiArray(1 , 2 ,50 ,3*1024*1024);
    //printf("user login\n");
    char format_q[255]="select `last_login_time`,`user_set` from `user_info` where `uname`='%s' and `pwd`='%s'";
    char my_q[255];
    //printf("%s\n",passwd);
    snprintf(my_q,255 ,format_q,name,md5(passwd).c_str());
    ret=mydb.MySelect(1,2,my_q,my_result,my_lengths);
    mydb.close_conn();
	char logd[100];
    sprintf(logd, "check login user: %s", name);
    xl.appendlog_sql_select("user_info", logd , ret);
    return ret;
}
void ServerToDatabase::UpdateLastLoginTime(const char* name,const char* now_time)
{
    int ret;
    mydb.get_conn();
    //printf("update last login time\n");
    //NewMultiArray(1 , 2 ,50 ,3*1024*1024);
    char format_q[255]="update `user_info` set `last_login_time`='%s' where `uname`='%s'";
    char my_q[255];
    snprintf(my_q,255,format_q,now_time,name);
    ret=mydb.MyNoSelect(my_q);
    xl.appendlog_sql_update("user_info", "last_login_time", name);

    mydb.close_conn();
    return;
}
void ServerToDatabase::AddUser(const char* name,const char* passwd,const char* last_login_time)
{
    int ret;
    MYSQL * conn;
    char* default_set=new char[1024*1024];
    char* chunk_set=new char[2*1024*1024+1];
    strcpy(default_set,"default");
    int set_size=7;
    int query_len;
    conn=mydb.get_conn();
    //printf("add user\n");
    mysql_real_escape_string(conn, chunk_set, default_set, set_size);
    char format_q[1024] = "insert into `user_info` values('%s','%s','%s','reviewLineCnt 120 \nfontsize 12 \nfontcolor 0\n')";
    char* my_q=new char[3*1024*1024];
    query_len=snprintf(my_q,3*1024*1024,format_q,name,md5(passwd).c_str(),last_login_time);
    ret=mydb.MyNoSelect(query_len,my_q);

    xl.appendlog_sql_insert("user_info", ret);

    strcpy(format_q,"create table %s_msgtable("\
                "um_id int unsigned primary key auto_increment,"\
                "send_time datetime not null,"\
                "from_user char(32) not null," \
                "to_user char(32) not null," \
                "cid int unsigned," \
                "ctype char(1) default 'm'," \
                "foreign key(`from_user`) references user_info(`uname`)" \
            ")engine=innodb,charset=utf8,collate=utf8_bin");
    snprintf(my_q,3*1024*1024,format_q,name);
    ret=mydb.MyNoSelect(my_q);

    char logdata[100];
    sprintf(logdata, "%s_msgtable", name);
    xl.appendlog_sql_create(logdata);

    mydb.close_conn();
    //printf("add userok \n");
    delete[] default_set;
    delete[] chunk_set;
    delete[] my_q;  
    return; 
}
bool ServerToDatabase::UpdateUserPwd(const char* name,const char* last_pwd,const char* now_pwd)
{
    if(!CheckUserLogin(name,(last_pwd))){
        return false;
    }
    int ret;
    mydb.get_conn();
    char format_q[255]="update `user_info` set `pwd`='%s' where `uname`='%s'";
    char my_q[255];
    snprintf(my_q,255,format_q,md5(now_pwd).c_str(),name);
    ret=mydb.MyNoSelect(my_q);
    mydb.close_conn();
    xl.appendlog_sql_update("user_info", "passwd", name);
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
    char format_q[1023]="update `user_info` set `user_set`='%s' where `uname`='%s'";
    char* my_q=new char[3*1024*1024];
    query_len=snprintf(my_q,3*1024*1024,format_q,chunk_set,name);
    ret=mydb.MyNoSelect(query_len,my_q);
    mydb.close_conn();
    xl.appendlog_sql_update("user_info", "user_set", name);
    delete[] chunk_set;
    delete[] my_q;
}
unsigned int ServerToDatabase::PushBackTextContain(const char* tcontain)
{
    int ret;
    MYSQL * conn;
    char *chunk_set = new char[2 * 1024 * 1024 + 1];
    int query_len;
    conn=mydb.get_conn();
    mysql_real_escape_string(conn, chunk_set, tcontain, strlen(tcontain));
    char format_q[1024]="insert into `text_contain` values(NULL,'%s')";
    char* my_q=new char[3*1024*1024];
    query_len=snprintf(my_q, 3 * 1024 * 1024, format_q, chunk_set);
    ret = mydb.MyNoSelect(query_len,my_q);
    mydb.close_conn();

    xl.appendlog_sql_insert("text_contain",ret);
    delete[] chunk_set;
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
    if(strcmp(fcontain,"")){
        mysql_real_escape_string(conn, chunk_fcontain, fcontain, fsize);
    }else{
        strcpy(chunk_fcontain,"");
    }
    char format_q[255]="insert into `file_contain` values(NULL, %d ,'%s','%s')";
    char* my_q=new char[3*1024*1024];
    query_len=snprintf(my_q,3*1024*1024,format_q,fsize,fname,chunk_fcontain);
    ret=mydb.MyNoSelect(query_len,my_q);
    mydb.close_conn();

    xl.appendlog_sql_insert("file_contain", ret);

    delete[] chunk_fcontain;
    delete[] my_q;
    return mydb.get_last_insert_id(); 
}
//todo
unsigned int ServerToDatabase::PushBackUserReceive(const char* uname,const char* send_time,const char* from_name,const char* to_name,const unsigned int& cid,const char& ctype)
{
    int ret;
    MYSQL * conn;
    conn=mydb.get_conn();
    char format_q[255]="insert into `%s_msgtable` values(NULL,'%s','%s','%s',%u,'%c')";
    char* my_q=new char[1024*1024];
    snprintf(my_q,1024*1024,format_q,uname,send_time,from_name,to_name,cid,ctype);
    ret=mydb.MyNoSelect(my_q);
    mydb.close_conn();

    char logdata[100];
    sprintf(logdata, "%s_msgtable",uname);
    xl.appendlog_sql_insert(logdata,ret);
    delete[] my_q;
    return mydb.get_last_insert_id();  
}
//todo
int ServerToDatabase::GetLastTexts(const int& row_num,const char* name,const char* op_name)
{
    int ret;
    mydb.get_conn();
    NewMultiArray(row_num+1 , 7 ,128 ,1024*1024);
    char format_q[512];
    char my_q[512];
    printf("op_name:%s,need_num:%d\n", op_name, row_num);
    if(!strcmp(op_name,"")){
        printf("all\n");
        strcpy(format_q, "select `send_time`,`from_user`,`to_user`,`ctype`,`fid`,`fname`,`mcontain` from %s_msgtable as ust left outer join text_contain on text_contain.`mid`=ust.`cid` left outer join file_contain on ust.`cid`= file_contain.`fid` "
                         "where to_user='' order by `send_time` desc limit 0,%d");
        snprintf(my_q,512,format_q,name,row_num);
    }else{
        strcpy(format_q, "select `send_time`,`from_user`,`to_user`,`ctype`,`fid`,`fname`,`mcontain` from %s_msgtable as ust left outer join text_contain on text_contain.`mid`=ust.`cid` left outer join file_contain on ust.`cid`= file_contain.`fid` "
                         "where (to_user='%s') or (from_user='%s' and (not to_user='')) order by `send_time` desc limit 0,%d");
        snprintf(my_q,512,format_q,name,op_name,op_name,row_num);
    }
    ret=mydb.MySelect(row_num,7,my_q,my_result,my_lengths);
    mydb.close_conn();
    char logdata[100];
    sprintf(logdata, "%s_msgtable", name);
    xl.appendlog_sql_select(logdata, "select records", ret);
    return ret;  
}
void ServerToDatabase::GetFile(const unsigned int& file_key,const char* name)
{
    int ret;
    mydb.get_conn();
    NewMultiArray( 1 , 6 ,128, 2*1024*1024+1);
    char format_q[512]="select `fsize`,`fname`,`from_user`,`to_user`,`send_time`,`fcontain` from file_contain join %s_msgtable as umsg on umsg.`cid`=file_contain.`fid` where `fid`=%u and `ctype`='f' limit 0,1";
    char my_q[512];   
    snprintf(my_q,512,format_q,name,file_key);
    ret=mydb.MySelect(1,6,my_q,my_result,my_lengths);
    mydb.close_conn();
    xl.appendlog_sql_select("file_contain", "select file form chat record", ret);
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
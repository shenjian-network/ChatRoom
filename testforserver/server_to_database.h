//Copyright 2018 Tongji
//License:GPL
//Author:zyc
//This is a database owned by server.
#ifndef SERVERTODATABASE
#define SERVERTODATABASE
#include "my_database.h"
class ServerToDatabase{
public:
    ServerToDatabase();
    char*** get_my_result();
    unsigned long** get_my_lengths();
    int GetUserInfo();//返回用户数量，将结果存在my_result里（包含name字段）
    bool CheckUserLogin(const char* name,const char* passwd);//返回是否登录成功，将结果存在my_result里（包含last_login_time字段、user_set字段）
    void UpdateLastLoginTime(const char* name,const char* now_time);
    void AddUser(const char* name,const char* passwd,const char* last_login_time);
    bool UpdateUserPwd(const char* name,const char* last_pwd,const char* now_pwd);
    void UpdateUserSet(const char* name,const char* uset,const int text_length);//更新文件配置
    unsigned int PushBackTextContain(const char* tcontain);//返回插入的id值,请保证尾零
    unsigned int PushBackFileContain(const int& fsize,const char* fname,const char* fcontain);//返回插入的id值
    unsigned int PushBackUserReceive(const char* to_name,const char* send_time,const char* from_name,const unsigned int& cid,const char& ctype);//返回插入的id值
    int GetLastTexts(const int& row_num,const char* name);//返回获得的行数，其结果时间从大到小依次排列，其字段依次为（send_time,from_user,ctype,fkey,fname,mcontain）
    void GetFile(const unsigned int& file_key,const char* name);//根据键值获得文件的内容和大小，其只包括字段(`fsize`,`fname`,`from_user`,`send_time`,`file_contain`)
private:
    void NewMultiArray(const int row,const int line,const int general_size,const int last_size);//
    void DeleteMultiArray();
    char*** my_result;//查询结果存储处
    unsigned long** my_lengths;//和my_result配合使用，指明各部分的长度（不含尾0）
    MyDatabase mydb;
    int last_new_row;
    int last_new_line;
};
#endif
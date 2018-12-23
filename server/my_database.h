//Copyright 2018 Tongji
//License:GPL
//Author:lsx zyc
//This is a database owned by server.
#ifndef MYDATABASE
#define MYDATABASE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <iostream> 
class MyDatabase{
public:
    MyDatabase();
    int MySelect(const int& row_num,const int& line_num,const char *q,char** r[100],unsigned long** length_all);//自行管理外部链接
    bool MyNoSelect(const char *q);//自行管理外部链接
    bool MyNoSelect(const int& l,const char *q);//自行管理外部链接
    unsigned int get_last_insert_id();//获得上次插入得到的id
    MYSQL* get_conn();//创建一个新的链接
    void close_conn();//销毁链接
private:
    void MyDatabaseDie();
    void MyQuery(MYSQL * conn,const char* s);
    void Init();
    static const char host[30];
    static const char user[30];
    static const char passwd[30];
    static const char charset[30];
    static const char database[30];
    unsigned int last_insert_id;
    MYSQL *conn;
};

#endif
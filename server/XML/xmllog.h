#ifndef XMLLOG
#define XMLLOG
#include <stdio.h>
#include "tinyxml.h"
#include <iostream>
#include <cstring>
#include <ctime>
using namespace std;

class XmlLog{
    char xmlfile[30];//xml文件名

public:
    XmlLog();
    XmlLog(const char* s);

    void xml_init();
    void appendlog_sql_select(const char *table_name, const char *operation, int sql_key);        //添加数据库操作日志
    void appendlog_sql_update(const char *table_name, const char *biaoxiang, const char *yuanzu); //添加数据库操作日志
    void appendlog_sql_insert(const char *table_name, int sql_key);  //添加数据库操作日志
    void appendlog_sql_create(const char *table_name);                                      //添加数据库操作日志
    void appendlog_sendText(const char *from, const int unum, char **to);
    void appendlog_sendFileInfo(const char *from, const int unum, char **to);
    void appendlog_send(const char *to, const char *packethead);               //添加server发送数据日志
    void appendlog_recv(const char *from, const char *packethead);             //添加server接收数据日志
    void appendlog_user_askfor_text(const char *username, const char *result, const int ret);       //添加用户登录日志
    void appendlog_user_login(const char *username, const char *result);       //添加用户登录日志
    void appendlog_user_logout(const char *username, const char *result);                                       //添加用户退出日志
    void appendlog_user_add(const char *username , const char *result);                    //添加新建用户日志
    void appendlog_passwd_change(const char *username, const char *result);                //添加密码修改日志
    void appendlog_userconfig_change(const char *username, const char *result);            //添加用户配置修改日志
    void appendlog_others(const char *data);                                                //添加其他事件日志
    void appendlog_onlinefile(const char *sname, const char *rname, const int bnum);
    void appendlog_onlinefilenotify(const char *sname, const char *rname,const int bnum, const char *msg);
};

extern XmlLog xl;

#endif
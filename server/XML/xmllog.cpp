#include "xmllog.h"


XmlLog::XmlLog()
{
    strcpy(xmlfile, "serverlog.xml");
    xml_init();
}

XmlLog::XmlLog(const char* s)
{
    strcpy(xmlfile, s);
    xml_init();
}

void XmlLog::xml_init()
{
    TiXmlDocument *writeDoc = new TiXmlDocument; //xml文档指针
    TiXmlDeclaration *decl = new TiXmlDeclaration("1.0", "UTF-8", "yes");
    writeDoc->LinkEndChild(decl); //写入文档

    TiXmlElement *RootElement = new TiXmlElement("Logs"); //根元素
    RootElement->SetAttribute("number", 0);
    writeDoc->LinkEndChild(RootElement);

    writeDoc->SaveFile(xmlfile);
    delete writeDoc;
}

void XmlLog::appendlog_onlinefilenotify(const char *sname, const char *rname, const int bnum, const char *msg)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "send onlinefile notify");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    subElement = new TiXmlElement("send_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(sname);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("recv_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(rname);
    subElement->LinkEndChild(Content);

    //sql操作对应的主码
    subElement = new TiXmlElement("notify");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(msg);
    subElement->LinkEndChild(Content);

    char a[10];
    char b[100];
    sprintf(a, "%d", bnum);
    if (strcmp(msg, "sender request for send file") == 0 || strcmp(msg, "sender cancel to file") == 0)
    {
        strcpy(b, "file_length");
    }
    else
    {
        strcpy(b, "block_num");
    }
    subElement = new TiXmlElement(b);
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(a);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

void XmlLog::appendlog_onlinefile(const char *sname, const char *rname, const int bnum)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "send onlinefile");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    subElement = new TiXmlElement("send_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(sname);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("recv_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(rname);
    subElement->LinkEndChild(Content);

    //sql操作对应的主码
    char a[10];
    sprintf(a, "%d", bnum);
    subElement = new TiXmlElement("block_num");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(a);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加数据库操作日志
void XmlLog::appendlog_sql_select(const char *table_name, const char *operation, int sql_key)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "sql select");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    char key[10];
    sprintf(key, "%d", sql_key);

    //sql操作的表名
    subElement = new TiXmlElement("table_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(table_name);
    subElement->LinkEndChild(Content);

    //sql操作的类型
    subElement = new TiXmlElement("operation");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(operation);
    subElement->LinkEndChild(Content);

    //sql操作对应的主码
    subElement = new TiXmlElement("select_num");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(key);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加数据库操作日志
void XmlLog::appendlog_sql_update(const char *table_name, const char *biaoxiang, const char *yuanzu)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "sql update");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    //sql操作的表名
    subElement = new TiXmlElement("table_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(table_name);
    subElement->LinkEndChild(Content);

    //sql操作的类型
    subElement = new TiXmlElement("entry");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(biaoxiang);
    subElement->LinkEndChild(Content);

    //sql操作对应的主码
    subElement = new TiXmlElement("key");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(yuanzu);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加数据库操作日志
void XmlLog::appendlog_sql_insert(const char *table_name,  int sql_key)
{
    TiXmlDocument mydoc(xmlfile);        //xml文档对象
    bool loadOk = mydoc.LoadFile();      //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);


    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now),24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "sql insert");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    char key[10];
    sprintf(key, "%d", sql_key);


    //sql操作的表名
    subElement = new TiXmlElement("table_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(table_name);
    subElement->LinkEndChild(Content);



    //sql操作对应的主码
    subElement = new TiXmlElement("insert_return ");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(key);
    subElement->LinkEndChild(Content);


    mydoc.SaveFile(xmlfile);
}

//数据库创建表日志
void XmlLog::appendlog_sql_create(const char *table_name)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "sql create table");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    //sql操作的表名
    subElement = new TiXmlElement("table_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(table_name);
    subElement->LinkEndChild(Content);


    mydoc.SaveFile(xmlfile);

}

//添加server发送数据日志
void XmlLog::appendlog_send(const char *to, const char *packethead)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "server端发送信息");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;



    char tablename[50];
    strcpy(tablename, to);
    strcat(tablename, "_msgtable");


    //发送对象
    subElement = new TiXmlElement("发送对象");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(to);
    subElement->LinkEndChild(Content);

    //sql操作的表名
    subElement = new TiXmlElement("报头");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(packethead);
    subElement->LinkEndChild(Content);

    //相关数据库
    subElement = new TiXmlElement("相关数据库");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(tablename);
    subElement->LinkEndChild(Content);



    mydoc.SaveFile(xmlfile);
}

//添加server接收数据日志
void XmlLog::appendlog_recv(const char *from, const char *packethead)
{
    printf("srecv2\n");
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "server端接收信息");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;



    char tablename[50];
    strcpy(tablename, from);
    strcat(tablename, "_msgtable");

    //发送对象
    subElement = new TiXmlElement("接受对象");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(from);
    subElement->LinkEndChild(Content);

    //sql操作的表名
    subElement = new TiXmlElement("报头");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(packethead);
    subElement->LinkEndChild(Content);

    //相关数据库
    subElement = new TiXmlElement("相关数据库");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(tablename);
    subElement->LinkEndChild(Content);



    mydoc.SaveFile(xmlfile);
}

//添加用户登录日志
void XmlLog::appendlog_user_login(const char *username, const char *result)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "user_login");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;


    //发送对象
    subElement = new TiXmlElement("user_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(username);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("login_result");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(result);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加用户退出日志
void XmlLog::appendlog_user_logout(const char *username, const char *result)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "user logout");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    //发送对象
    subElement = new TiXmlElement("user_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(username);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("logout_result");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(result);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加新建用户日志
void XmlLog::appendlog_user_add(const char *username, const char *result)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "user_register");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    //发送对象
    subElement = new TiXmlElement("user_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(username);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("register_result");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(result);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加密码修改日志
void XmlLog::appendlog_passwd_change(const char *username, const char *result)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    printf("change pass11\n");
    if (!loadOk)
    {
        printf("change pass22\n");
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }
    printf("change pass33\n");
    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "user update passwd");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;


    //发送对象
    subElement = new TiXmlElement("user_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(username);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("update_passwd_result");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(result);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加用户配置修改日志
void XmlLog::appendlog_userconfig_change(const char *username, const char *result)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "user update set config");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    //发送对象
    subElement = new TiXmlElement("user");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(username);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("update_config_result");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(result);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加其他事件日志
void XmlLog::appendlog_others(const char *data)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "others");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;


    //发送对象
    subElement = new TiXmlElement("data");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(data);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

//添加server发送数据日志
void XmlLog::appendlog_sendText(const char *from, const int unum, char **to)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "Send Text Message");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;


    //发送对象
    subElement = new TiXmlElement("from");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(from);
    subElement->LinkEndChild(Content);

    for (int i = 0; i < unum;i++)
    {
        subElement = new TiXmlElement("to");
        LogElement->LinkEndChild(subElement);
        if(unum==1 && to[0][0]=='\0')
            Content = new TiXmlText("all");
        else
            Content = new TiXmlText(to[i]);
        subElement->LinkEndChild(Content);
    }

    mydoc.SaveFile(xmlfile);
}

//添加server发送数据日志
void XmlLog::appendlog_sendFileInfo(const char *from, const int unum, char **to)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "Send FileInfo Message");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    //发送对象
    subElement = new TiXmlElement("from");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(from);
    subElement->LinkEndChild(Content);

    for (int i = 0; i < unum; i++)
    {
        subElement = new TiXmlElement("to");
        LogElement->LinkEndChild(subElement);
        Content = new TiXmlText(to[i]);
        subElement->LinkEndChild(Content);
    }

    mydoc.SaveFile(xmlfile);
}

//添加其他事件日志
void XmlLog::appendlog_user_askfor_text(const char *username, const char *result, const int ret)
{
    TiXmlDocument mydoc(xmlfile);   //xml文档对象
    bool loadOk = mydoc.LoadFile(); //加载文档
    if (!loadOk)
    {
        cout << "could not load the test file.Error:" << mydoc.ErrorDesc() << endl;
        exit(1);
    }

    TiXmlElement *RootElement = mydoc.RootElement(); //获取根元素
    TiXmlAttribute *p = RootElement->FirstAttribute();
    char s[10];
    strcpy(s, p->Value());
    int num = atoi(s);
    num++;
    RootElement->SetAttribute("number", num);

    //获取时间
    time_t now = time(0);
    // 把 now 转换为字符串形式
    char dt[25];
    memcpy(dt, ctime(&now), 24);
    dt[24] = '\0';

    TiXmlElement *LogElement = new TiXmlElement("Log");
    LogElement->SetAttribute("Event", "server send chat record");
    LogElement->SetAttribute("Time", dt);
    RootElement->LinkEndChild(LogElement); //父节点写入文档

    TiXmlElement *subElement;
    TiXmlText *Content;

    //发送对象
    subElement = new TiXmlElement("user_name");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(username);
    subElement->LinkEndChild(Content);

    subElement = new TiXmlElement("result");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(result);
    subElement->LinkEndChild(Content);

    char dd[10];
    sprintf(dd, "%d", ret);
    subElement = new TiXmlElement("record_num");
    LogElement->LinkEndChild(subElement);
    Content = new TiXmlText(dd);
    subElement->LinkEndChild(Content);

    mydoc.SaveFile(xmlfile);
}

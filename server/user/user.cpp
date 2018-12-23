#include "user.h"

XmlLog xl;

/*SocketFunction部分*/
const int SocketFunction::my_port = 25998;      //TODO
const char *SocketFunction::my_ip = "0.0.0.0"; //TODO
void SocketFunction::Die(const char *msg)
{
    perror(msg);
    exit(1);
}

void SocketFunction::SetSockNonBlock(int sock)
{
    int flags;
    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0)
    {
        Die("fcntl(F_GETFL) failed");
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        Die("fcntl(F_SETFL) failed");
    }
}

int SocketFunction::InitSocket()
{
    int serversock, flag;
    sockaddr_in echoserver;
    if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        Die("Failed to create socket");
    }
    SetSockNonBlock(serversock);
    memset(&echoserver, 0, sizeof(echoserver));
    echoserver.sin_family = AF_INET;
    echoserver.sin_addr.s_addr = inet_addr(my_ip);
    echoserver.sin_port = htons(my_port);
    if (setsockopt(serversock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) == -1)
    {
        Die("set socket error");
    }
    /* 绑定端口 */
    if (bind(serversock, (struct sockaddr *)&echoserver, sizeof(echoserver)) < 0)
    {
        Die("Failed to bind the server socket");
    }
    /* 设置监听模式 */
    if (listen(serversock, 1023) < 0)
    {
        Die("Failed to listen on server socket");
    }
    return serversock;
}

int SocketFunction::MyRecvC(const int &sock, const int &required_size, char *buffer)
{
    int need_s = required_size;
    int RecvSize;
    while (need_s > 0)
    {
        RecvSize = recv(sock, buffer, need_s, MSG_DONTWAIT);
        if (RecvSize < 0)
        {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                continue;
            }
            printf("client error:%d\n",errno);
            return 0;
        }
        else if (RecvSize == 0)
        {
            printf("link interrupted\n");
            return 0;
        }
        need_s = need_s - RecvSize;
        buffer += RecvSize;
    }
    return required_size;
}

/*User部分*/
//建立一个空用户，用于注册时使用
User::User()
{
    Clear();
}

//在系统开始运行时，为数据库中原有的用户建立User结构
User::User(const char *uname)
{
    set_user_name(uname);
    set_user_sockfd(-1);
    set_user_status(false);
}

char *User::get_user_name()
{
    return name;
}

//
bool User::get_user_status()
{
    return online;
}

//
int User::get_user_sockfd()
{
    return sock_fd;
}
//
void User::set_user_status(bool b)
{
    online = b;
}

//
void User::set_user_sockfd(int i)
{
    sock_fd = i;
}

void User::set_user_name(const char *n)
{
    strcpy(name, n);
}

bool User::SendMessage(ServerToClientBase *scb)
{
    int size = 8 + scb->get_packet_head().get_length();
    char *tmps = new char[size + 1];
    scb->get_string(tmps);
    char *buffer = tmps;
    int err_count = 0;
    int SendSize;
    if (sock_fd < 0)
    {
        printf("user: unexpected interrupted\n");
        return false;
    }
    while (size > 0)
    {
        SendSize = send(sock_fd, buffer, size, MSG_DONTWAIT);
        if (SendSize <= 0)
        {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                // if ((++err_count) > error_max_number)
                // {
                //     delete[] tmps;
                //     return false;
                // }
                continue;
            }
            else
            {
                delete[] tmps;
                return false;
            }
        }
        size = size - SendSize;
        buffer += SendSize;
    }
    delete[] tmps;
    return true;
}

bool User::SendMessage(ClientToServerBase *csb)
{
    int size = 8 + csb->get_packet_head().get_length();
    char *tmps = new char[size + 1];
    csb->get_string(tmps);
    char *buffer = tmps;
    int err_count = 0;
    int SendSize;
    while (size > 0)
    {
        SendSize = send(sock_fd, buffer, size, MSG_DONTWAIT);
        if (SendSize <= 0)
        {
            if ((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
            {
                continue;
            }
            else
            {
                delete[] tmps;
                return false;
            }
        }
        size = size - SendSize;
        buffer += SendSize;
    }
    delete[] tmps;
    return true;
}

void User::Clear()
{
    set_user_name("");
    set_user_sockfd(-1);
    set_user_status(false);
}

bool operator==(const User &ua, const User &ub)
{
    return (!strcmp(ua.name, ub.name));
}

bool operator==(const User &ua, const char *cb)
{
    return (!strcmp(ua.name, cb));
}
/*FileNode部分*/
FileNode::FileNode(const char* fn,int fs)
{
    strcpy(file_name,fn);
    file_size=fs;
}
bool operator==(const FileNode& fna,const char* fnb)
{
    return (!strcmp(fna.file_name,fnb));
}
/*UserManager部分*/
void UserManager::get_now_time(char *&bufferWrite)
{
    //Get formatted time into bufferWrite
    time_t now = time(NULL);
    tm t;
    localtime_r(&now, &t);
    strftime(bufferWrite, 20, "%Y-%m-%d %H:%M:%S", &t);
}
const char tmp_for_set[] = {'!', '@', '#', '^', '&', '*', '(', ')'};
const set<char> legal_char_set(tmp_for_set, tmp_for_set + sizeof(tmp_for_set));
bool UserManager::CheckStr(const char *s)
{
    for (int i = 0; s[i]; i++)
    {
        if ((legal_char_set.find(s[i]) == legal_char_set.end()) && (s[i] > '9' && s[i] < '0') && (s[i] < 'a' && s[i] > 'z') && (s[i] < 'A' && s[i] > 'Z'))
        {
            return false;
        }
    }
    return true;
}
/*UserManager:CreateSection*/
ServerToClientBase *UserManager::CreateRetReportLoginPacket(ClientToServerReportLogin *cl, char*& set_contain, unsigned long &user_set_length)
{
    ServerToClientBase *ret_pack;
    PacketHead rec_head = cl->get_packet_head();
    PacketHead ret_head;
    bool check_res;
    char ***db_results;
    unsigned long **db_lengths;
    vector<User>::iterator u_it;
    char *now_time = new char[21];
    int user_num = my_clients.size();
    char **users_info = new char *[user_num];
    for (int i = 0; i < user_num; i++)
    {
        users_info[i] = new char[34];
        memset(users_info[i], 0, 33 * sizeof(char));
        strcpy(users_info[i], my_clients[i].get_user_name());
        users_info[i][32] = '0' + (int)my_clients[i].get_user_status();
        if (!strcmp(my_clients[i].get_user_name(), cl->get_user_name()))
        {
            users_info[i][32] = '1';
        }
    }
    ret_head.set_packet_type(PacketHead::kS2CReport);
    ret_head.set_length(0);
    u_it = find(my_clients.begin(), my_clients.end(), cl->get_user_name());
    set_contain = NULL;
    user_set_length = 0;
    if (rec_head.get_function_type() == PacketHead::kC2SReportLoginIn)
    {
        if (u_it == my_clients.end())
        {
            ret_head.set_function_type(PacketHead::kS2CReportNoExist);
            ret_pack = new ServerToClientBase(ret_head);
            xl.appendlog_user_login(cl->get_user_name(),"login failed, user not exist");
        }
        else
        {
            check_res = mydb.CheckUserLogin(cl->get_user_name(), cl->get_user_pwd());
            db_results = mydb.get_my_result();
            db_lengths = mydb.get_my_lengths();
            if (check_res)
            {
                if (u_it->get_user_status())
                {
                    ret_head.set_function_type(PacketHead::kS2CReportSuccessDup);
                    ret_head.set_length(19 + 4 + 33 * user_num);
                    ret_pack = new ServerToClientReportSuccess(ret_head, db_results[0][0], user_num, users_info);
                    xl.appendlog_user_login(cl->get_user_name(), "login ok, but user is already online");
                }
                else
                {
                    if (db_results[0][0][0] == '0')
                    {
                        ret_head.set_function_type(PacketHead::kS2CReportMustUpdate);
                        ret_pack = new ServerToClientBase(ret_head);
                        xl.appendlog_user_login(cl->get_user_name(), "login failed, check user and password ok, but it's the first login time, need to update the password");
                    }
                    else
                    {
                        ret_head.set_function_type(PacketHead::kS2CReportSuccess);
                        ret_head.set_length(19 + 4 + 33 * user_num);
                        ret_pack = new ServerToClientReportSuccess(ret_head, db_results[0][0], user_num, users_info);
                        xl.appendlog_user_login(cl->get_user_name(), "login success");
                    }
                }
            }
            else
            {
                ret_head.set_function_type(PacketHead::kS2CReportWrongPwd);
                ret_pack = new ServerToClientBase(ret_head);
                xl.appendlog_user_login(cl->get_user_name(), "login failed,check user and password failed");
            }
        }
        if ((ret_head.get_function_type() == PacketHead::kS2CReportSuccessDup) || (ret_head.get_function_type() == PacketHead::kS2CReportSuccess))
        {
            get_now_time(now_time);
            mydb.UpdateLastLoginTime(cl->get_user_name(), now_time);
            user_set_length = db_lengths[0][1];
            set_contain = db_results[0][1];
        }
    }
    else
    {
        if (u_it != my_clients.end())
        {
            ret_head.set_function_type(PacketHead::kS2CReportDuplicated);
            xl.appendlog_user_add(cl->get_user_name(), "register failed, user is already exist");
        }
        else
        {
            if (!CheckStr(cl->get_user_name()))
            {
                ret_head.set_function_type(PacketHead::kS2CReportNameNotAccess);
                xl.appendlog_user_add(cl->get_user_name(), "register failed, user name don't meet the requirements");
            }
            else if (!CheckStr(cl->get_user_pwd()))
            {
                ret_head.set_function_type(PacketHead::kS2CReportPwdNotAccess);
                xl.appendlog_user_add(cl->get_user_name(), "register failed, password don't meet the requirements");
            }
            else
            {
                ret_head.set_function_type(PacketHead::kS2CReportSuccess);
                get_now_time(now_time);
                mydb.AddUser(cl->get_user_name(), cl->get_user_pwd(), now_time);
                xl.appendlog_user_add(cl->get_user_name(), "register success");
            }
        }
        ret_pack = new ServerToClientBase(ret_head);
    }
    delete[] now_time;
    for (int i = 0; i < user_num; i++)
    {
        delete[] users_info[i];
    }
    delete[] users_info;
    return ret_pack;
}
ServerToClientBase *UserManager::CreateRetReportUpdatePacket(ClientToServerReportUpdate *cu)
{
    ServerToClientBase *ret_pack;
    PacketHead ret_head;
    bool check_res;
    vector<User>::iterator u_it;
    char *now_time = new char[21];
    ret_head.set_packet_type(PacketHead::kS2CReport);
    ret_head.set_length(0);
    u_it = find(my_clients.begin(), my_clients.end(), cu->get_user_name());
    if (u_it == my_clients.end())
    {
        ret_head.set_function_type(PacketHead::kS2CReportNoExist);
        xl.appendlog_passwd_change(cu->get_user_name(), "update failed, user is not exist");
    }
    else
    {
        if (!CheckStr(cu->get_now_user_pwd())||(!strcmp(cu->get_now_user_pwd(),cu->get_last_user_pwd())))
        {
            ret_head.set_function_type(PacketHead::kS2CReportNowPwdNotAccess);
            xl.appendlog_passwd_change(cu->get_user_name(), "update failed, new passwd doesn't meet the requirement");
        }
        else
        {
            check_res = mydb.UpdateUserPwd(cu->get_user_name(), cu->get_last_user_pwd(), cu->get_now_user_pwd());
            if (check_res)
            {
                ret_head.set_function_type(PacketHead::kS2CReportUpdateSucess);
                get_now_time(now_time);
                mydb.UpdateLastLoginTime(cu->get_user_name(), now_time);
                xl.appendlog_passwd_change(cu->get_user_name(), "update sucess");
            }
            else
            {
                ret_head.set_function_type(PacketHead::kS2CReportLastPwdWrong);
                xl.appendlog_passwd_change(cu->get_user_name(), "update failed, old passwd is wrong");
            }
        }
    }
    ret_pack = new ServerToClientBase(ret_head);
    delete[] now_time;
    return ret_pack;
}
ServerToClientBase *UserManager::CreateRetSetPacket(const char *ftname, char *set_contain, unsigned long &user_set_length)
{
    ServerToClientBase *ret_pack;
    PacketHead ret_head;
    ret_head.set_packet_type(PacketHead::kS2CUserSet);
    ret_head.set_length(user_set_length);
    ret_head.set_function_type(PacketHead::kS2CUserSetUpdate);
    //xl.appendlog_userconfig_change();
    ret_pack = new ServerToClientUserSetUpdate(ret_head, set_contain);
    return ret_pack;
}
ServerToClientBase *UserManager::CreateRetInformPacket(const bool &is_online, const char *fname)
{
    ServerToClientBase *ret_pack;
    PacketHead ret_head;
    ret_head.set_packet_type(PacketHead::kS2CInform);
    ret_head.set_length(32);
    if (is_online)
    {
        ret_head.set_function_type(PacketHead::kS2CInformOnline);
    }
    else
    {
        ret_head.set_function_type(PacketHead::kS2CInformOffline);
        xl.appendlog_user_logout(fname, "user is logout");
    }
    ret_pack = new ServerToClientInform(ret_head, fname);
    return ret_pack;
}
ServerToClientBase** UserManager::CreateRetTextContain(ClientToServerTextToUsers *ct, const char *frname)
{
    ServerToClientBase **ret_pack;
    PacketHead ret_head;
    char *now_time = new char[21];
    unsigned int tid;
    int to_users_num = ct->get_user_num();
    char **to_users_name = ct->get_user_info();
    ret_head.set_packet_type(PacketHead::kS2CText);
    ret_head.set_function_type(PacketHead::kS2CTextSimpleText);
    ret_head.set_length(ct->get_text_length() + 32 + 19 + 32);
    get_now_time(now_time);
    tid = mydb.PushBackTextContain(ct->get_text_info());
    ret_pack=new ServerToClientBase*[to_users_num];
    for (int i = 0; i < to_users_num; i++)
    {
        ret_pack[i] = new ServerToClientTextSimpleText(ret_head, frname,to_users_name[i], now_time, ct->get_text_info());
        //TODO
        //mydb.PushBackUserReceive(to_users_name[i], now_time, frname, tid, 'm');
    }
    if(!strcmp(to_users_name[0],"")){
        //to all
        for(auto client:my_clients){
            mydb.PushBackUserReceive(client.get_user_name(), now_time, frname, "" , tid, 'm');
        }
    }else{
        for(int i=0;i<to_users_num;i++){
            mydb.PushBackUserReceive(to_users_name[i], now_time, frname,to_users_name[i], tid, 'm');
            mydb.PushBackUserReceive(frname, now_time, frname,to_users_name[i], tid, 'm');
        }
    }
    xl.appendlog_sendText(frname, to_users_num, to_users_name);
    delete[] now_time;
    return ret_pack;
}
ServerToClientBase **UserManager::CreateRetFileInfo(ClientToServerTextFileToUsers *cfi, const char *frname)
{
    ServerToClientBase **ret_pack;
    PacketHead ret_head;
    char *now_time = new char[21];
    unsigned int fid;
    int to_users_num = cfi->get_user_num();
    char **to_users_name = cfi->get_user_info();
    ret_head.set_packet_type(PacketHead::kS2CText);
    ret_head.set_function_type(PacketHead::kS2CTextFileInfo);
    ret_head.set_length(32 + 19 + 64 + 4 + 32);
    get_now_time(now_time);
    fid = mydb.PushBackFileContain(cfi->get_text_length(), cfi->get_file_name(), cfi->get_text_info());
    //todo
    ret_pack=new ServerToClientBase*[to_users_num];
    for (int i = 0; i < to_users_num; i++)
    {
        ret_pack[i] = new ServerToClientTextFileInfo(ret_head, frname,to_users_name[i], now_time, cfi->get_file_name(), fid);
    }    
    if(!strcmp(to_users_name[0],"")){
        //to all
        for(auto client:my_clients){
            mydb.PushBackUserReceive(client.get_user_name(), now_time, frname, "" , fid, 'f');
        }
    }else{
        for (int i = 0; i < to_users_num; i++)
        {
            mydb.PushBackUserReceive(to_users_name[i], now_time, frname,to_users_name[i], fid, 'f');
            mydb.PushBackUserReceive(frname, now_time, frname,to_users_name[i], fid, 'f');
        }
    }
    xl.appendlog_sendFileInfo(frname, to_users_num, to_users_name);
    delete[] now_time;
    return ret_pack;
}
ServerToClientBase *UserManager::CreateRetFileContain(ClientToServerTextAskForFile *cfc, const char *uname)
{
    ServerToClientBase *ret_pack;
    PacketHead ret_head;
    char ***db_results;
    unsigned long **db_lengths;
    int file_size;
    ret_head.set_packet_type(PacketHead::kS2CText);
    ret_head.set_function_type(PacketHead::kS2CTextFileContain);
    mydb.GetFile(cfc->get_file_key(), uname);
    db_results = mydb.get_my_result();
    db_lengths = mydb.get_my_lengths();
    file_size = atoi(db_results[0][0]);
    ret_head.set_length(32 + 19 + 64 + 4 + file_size + 32);
    //(`fsize`,`fname`,`from_user`,`to_user`,`send_time`,`file_contain`)
    ret_pack = new ServerToClientTextFileContain(ret_head, db_results[0][2],db_results[0][3], db_results[0][4], db_results[0][1], cfc->get_file_key(), db_results[0][5]);
    return ret_pack;
}
ServerToClientBase *UserManager::CreateRetCls()
{
    ServerToClientBase *ret_pack;
    PacketHead ret_head;
    ret_head.set_packet_type(PacketHead::kS2CText);
    ret_head.set_function_type(PacketHead::kS2CTextAskForClr);
    ret_head.set_length(0);
    ret_pack = new ServerToClientBase(ret_head);
    return ret_pack;
}
ServerToClientBase **UserManager::CreateRetAskForTexts(int &real_num, ClientToServerTextAskForTexts *cts, const char *uname)
{
    ServerToClientBase **ret_pack;
    PacketHead ret_head;
    char ***db_results;
    unsigned long **db_lengths;
    unsigned int f_key;
    real_num = mydb.GetLastTexts(cts->get_list_num(), uname , cts->get_op_user());
    db_results = mydb.get_my_result();
    db_lengths = mydb.get_my_lengths();
    ret_pack = new ServerToClientBase *[real_num];
    ret_head.set_packet_type(PacketHead::kS2CText);
    //（send_time,from_user,to_user,ctype,fkey,fname,mcontain）
    for (int i = real_num - 1; i >= 0; i--)
    {
        if (db_results[i][3][0] == 'm')
        {
            ret_head.set_function_type(PacketHead::kS2CTextSimpleText);
            ret_head.set_length(32 + 19+32 + db_lengths[i][6]);
            ret_pack[real_num - 1 - i] = new ServerToClientTextSimpleText(ret_head, db_results[i][1],db_results[i][2], db_results[i][0], db_results[i][6]);
        }
        else
        {
            ret_head.set_function_type(PacketHead::kS2CTextFileInfo);
            ret_head.set_length(19 + 32+32 + 64 + 4);
            f_key = atoi(db_results[i][4]);
            ret_pack[real_num - 1 - i] = new ServerToClientTextFileInfo(ret_head, db_results[i][1],db_results[i][2], db_results[i][0], db_results[i][5], f_key);
        }
    }
    xl.appendlog_user_askfor_text(uname, "request records", real_num);
    return ret_pack;
}
void UserManager::CreateRetSetPacket(ClientToServerUserSetUpdate *cs, const char *fname)
{
    mydb.UpdateUserSet(fname, cs->get_user_set_data(), cs->get_text_length());
    xl.appendlog_userconfig_change(fname, "update config sucess");
    return;
}
UserManager::UserManager() : mydb()
{
    // ServerToClientBase* t=CreateRetCls();
    // printf("%d\n",t->get_packet_head().get_packet_type());
    // printf("%d\n",t->get_packet_head().get_function_type());
    // char * s=new char[100];
    // t->get_string(s);
    // for(int i=0;i<8;i++){
    //     printf("%d\n",s[i]);
    // }
    //ServerToClientBase* s;

    signal(SIGPIPE, SIG_IGN);
    int users_num;
    User tmp_user;
    my_links = new LinkUnit[1024];
    char ***db_results;
    users_num = mydb.GetUserInfo(); //name
    db_results = mydb.get_my_result();
    for (int i = 0; i < users_num; i++)
    {
        tmp_user.set_user_name(db_results[i][0]);
        my_clients.push_back(tmp_user);
    }
    server_sock = SocketFunction::InitSocket();
}
UserManager::~UserManager()
{
    delete[] my_links;
}

//Report
bool UserManager::ClientSign(const int &client, const PacketHead& p,const char *b)
{
    if (my_links[client].get_user_status())
    {
        return true;
    }
    ClientToServerReportLogin *c_pack_login;
    ServerToClientBase *pack_ret;
    ServerToClientBase *pack_set_ret;
    ServerToClientBase *pack_online;
    ServerToClientBase *pack_offline;
    vector<User>::iterator uit;
    int dup_sock;
    char *set_contain=NULL;
    unsigned long user_set_length;
    c_pack_login = new ClientToServerReportLogin();
    c_pack_login->set_string(p,b);
    //printf("sign1\n");
    pack_ret = CreateRetReportLoginPacket(c_pack_login, set_contain, user_set_length);
    if (!my_links[client].SendMessage(pack_ret))
    {
        printf("send error\n");
        delete c_pack_login;
        delete pack_ret;
        return false;
    }
    //printf("1\n");
    my_links[client].set_user_name(c_pack_login->get_user_name());
    uit = find(my_clients.begin(), my_clients.end(), my_links[client].get_user_name());
    if (pack_ret->get_packet_head().get_function_type() == PacketHead::kS2CReportSuccessDup)
    {
        dup_sock = uit->get_user_sockfd();
        pack_offline = CreateRetInformPacket(0, my_links[client].get_user_name());
        my_links[dup_sock].SendMessage(pack_offline);
        //printf("2\n");
        my_links[dup_sock].set_user_status(false);
        delete pack_offline;
    }
    if (pack_ret->get_packet_head().get_function_type() == PacketHead::kS2CReportSuccess || pack_ret->get_packet_head().get_function_type() == PacketHead::kS2CReportSuccessDup)
    {
        uit->set_user_sockfd(client);
        uit->set_user_status(false);
        pack_online = CreateRetInformPacket(1, my_links[client].get_user_name());
        for (auto cu : my_clients)
        {
            if (cu.get_user_status())
            {
                my_links[cu.get_user_sockfd()].SendMessage(pack_online);
                //printf("3\n");
            }
        }
        my_links[client].set_user_status(true);
        uit->set_user_status(true);
        delete pack_online;
    }
    if ((set_contain == NULL)||(user_set_length==0))
    {
        return true;
    }
    pack_set_ret = CreateRetSetPacket(my_links[client].get_user_name(), set_contain, user_set_length);
    if (!my_links[client].SendMessage(pack_set_ret))
    {
        delete c_pack_login;
        delete pack_ret;
        delete pack_set_ret;
        return false;
    }
    //("4\n");
    delete c_pack_login;
    delete pack_ret;
    delete pack_set_ret;
    return true;
}
bool UserManager::ClientRegister(const int &client, const PacketHead& p,const char *b)
{
    ClientToServerReportLogin *c_pack_login;
    ServerToClientBase *pack_ret;
    ServerToClientBase *pack_offline;
    char *set_contain;
    unsigned long user_set_length;
    c_pack_login = new ClientToServerReportLogin();
    c_pack_login->set_string(p,b);
    pack_ret = CreateRetReportLoginPacket(c_pack_login, set_contain, user_set_length);
    if (!my_links[client].SendMessage(pack_ret))
    {
        delete c_pack_login;
        delete pack_ret;
        return false;
    }
    my_links[client].set_user_name(c_pack_login->get_user_name());
    if (pack_ret->get_packet_head().get_function_type() == PacketHead::kS2CReportSuccess)
    {
        my_clients.push_back(User(my_links[client].get_user_name()));
        pack_offline = CreateRetInformPacket(0, my_links[client].get_user_name());
        for (auto cu : my_clients)
        {
            if (cu.get_user_status())
            {
                my_links[cu.get_user_sockfd()].SendMessage(pack_offline);
            }
        }
        delete pack_offline;
    }
    delete c_pack_login;
    delete pack_ret;
    return true;
}
bool UserManager::ClientUpdate(const int &client,const PacketHead& p, const char *b)
{
    ClientToServerReportUpdate *c_pack_login;
    ServerToClientBase *pack_ret;
    ServerToClientBase *pack_offline;
    vector<User>::iterator uit;
    int dup_sock;
    c_pack_login = new ClientToServerReportUpdate();
    c_pack_login->set_string(p,b);
    pack_ret = CreateRetReportUpdatePacket(c_pack_login);
    if (!my_links[client].SendMessage(pack_ret))
    {
        delete c_pack_login;
        delete pack_ret;
        return false;
    }
    my_links[client].set_user_name(c_pack_login->get_user_name());
    uit = find(my_clients.begin(), my_clients.end(), my_links[client].get_user_name());
    if (pack_ret->get_packet_head().get_function_type() == PacketHead::kS2CReportUpdateSucess)
    {
        if (uit->get_user_status())
        {
            dup_sock = uit->get_user_sockfd();
            my_links[dup_sock].set_user_status(false);
            pack_offline = CreateRetInformPacket(0, my_links[client].get_user_name());
            for (auto cu : my_clients)
            {
                if (cu.get_user_status())
                {
                    my_links[cu.get_user_sockfd()].SendMessage(pack_offline);
                }
            }
            delete pack_offline;
            uit->set_user_status(false);
        }
    }
    delete c_pack_login;
    delete pack_ret;
    return true;
}
//Text
bool UserManager::ClientTextToUsers(const int &client, const PacketHead& p,const char *b)
{
    if (!my_links[client].get_user_status())
    {
        return true;
    }
    ClientToServerTextToUsers *c_pack;
    ServerToClientBase **pack_ret;
    vector<User>::iterator u1it;
    vector<User>::iterator u2it;
    int users_num;
    char **users_info;
    int send_sock;
    c_pack = new ClientToServerTextToUsers();
    c_pack->set_string(p,b);
    pack_ret = CreateRetTextContain(c_pack, my_links[client].get_user_name());
    // if (!my_links[client].SendMessage(pack_ret))
    // {
    //     delete c_pack;
    //     delete pack_ret;
    //     return false;
    // }
    users_num = c_pack->get_user_num();
    users_info = c_pack->get_user_info();
    u2it=find(my_clients.begin(),my_clients.end(),my_links[client].get_user_name());
    if(u2it!=my_clients.end()){
        u2it->set_user_status(false);
    }
    if(!strcmp(users_info[0],"")){
        for(auto client:my_clients){
            if(client.get_user_status()){
                 my_links[client.get_user_sockfd()].SendMessage(pack_ret[0]);
                 //printf("5\n");
            }
        }
    }else{
        for (int i = 0; i < users_num; i++)
        {
            u1it = find(my_clients.begin(), my_clients.end(), users_info[i]);
            if (u1it != my_clients.end())
            {
                if (u1it->get_user_status())
                {
                    send_sock = u1it->get_user_sockfd();
                    my_links[send_sock].SendMessage(pack_ret[i]);
                    //printf("6\n");
                }
            }
        }
    }
    if(u2it!=my_clients.end()){
        u2it->set_user_status(true);
    }
   // xl.appendlog_send(my_links[client].get_user_name(), "ClientTextToUsers");
    delete c_pack;
    //delete pack_ret;
    for(int i=0;i<users_num;i++){
        delete pack_ret[i];
    }
    delete[] pack_ret;
    return true;
}
bool UserManager::ClientFileToUsers(const int &client, const PacketHead& p,const char *b)
{
    if (!my_links[client].get_user_status())
    {
        return true;
    }
    ClientToServerTextFileToUsers *c_pack;
    ServerToClientBase **pack_ret;
    vector<User>::iterator u1it;
    vector<User>::iterator u2it;
    int users_num;
    char **users_info;
    int send_sock;
    c_pack = new ClientToServerTextFileToUsers();
    c_pack->set_string(p,b);
    pack_ret = CreateRetFileInfo(c_pack, my_links[client].get_user_name());
    // if (!my_links[client].SendMessage(pack_ret))
    // {
    //     delete c_pack;
    //     delete pack_ret;
    //     return false;
    // }
    users_num = c_pack->get_user_num();
    users_info = c_pack->get_user_info();
    u1it=find(my_clients.begin(),my_clients.end(),my_links[client].get_user_name());
    if(u1it!=my_clients.end()){
        u1it->set_user_status(false);
    }
    if(!strcmp(users_info[0],"")){
        for(auto client:my_clients){
            if(client.get_user_status()){
                 my_links[client.get_user_sockfd()].SendMessage(pack_ret[0]);
                 //printf("7\n");
            }
        }
    }else{
        for (int i = 0; i < users_num; i++)
        {
            u2it = find(my_clients.begin(), my_clients.end(), users_info[i]);
            if (u2it != my_clients.end())
            {
                if (u2it->get_user_status())
                {
                    send_sock = u2it->get_user_sockfd();
                    my_links[send_sock].SendMessage(pack_ret[i]);
                    //printf("8\n");
                }
            }
        }
    }
    if(u1it!=my_clients.end()){
        u1it->set_user_status(true);
    }
    //xl.appendlog_send(my_links[client].get_user_name(), "ClientFileToUsers");
    delete c_pack;
    //delete pack_ret;
    for(int i=0;i<users_num;i++){
        delete pack_ret[i];
    }
    delete[] pack_ret;
    return true;
}
bool UserManager::ClientTextAskForFile(const int &client,const PacketHead& p, const char *b)
{
    if (!my_links[client].get_user_status())
    {
        return true;
    }
    ClientToServerTextAskForFile *c_pack;
    ServerToClientBase *pack_ret;
    c_pack = new ClientToServerTextAskForFile();
    c_pack->set_string(p,b);
    pack_ret = CreateRetFileContain(c_pack, my_links[client].get_user_name());
    if (!my_links[client].SendMessage(pack_ret))
    {
        delete c_pack;
        delete pack_ret;
        return false;
    }
    //printf("9\n");
    //xl.appendlog_recv(my_links[client].get_user_name(), "ClientTextAskForFile");
    delete c_pack;
    delete pack_ret;
    return true;
}
bool UserManager::ClientTextAskForTexts(const int &client,const PacketHead& p, const char *b)
{
    if (!my_links[client].get_user_status())
    {
        return true;
    }
    int real_num = 0;
    int error_flag = 0;
    ClientToServerTextAskForTexts *c_pack;
    ServerToClientBase **pack_ret;
    ServerToClientBase *pack_cls;
    c_pack = new ClientToServerTextAskForTexts();
    c_pack->set_string(p,b);
    pack_cls = CreateRetCls();
    pack_ret = CreateRetAskForTexts(real_num, c_pack, my_links[client].get_user_name());
    if (!my_links[client].SendMessage(pack_cls))
    {
        error_flag = 1;
    }
    //printf("10\n");
    for (int i = 0; i < real_num; i++)
    {
        if (error_flag)
        {
            break;
        }
        if (!my_links[client].SendMessage(pack_ret[i]))
        {
            error_flag = 1;
        }
        //printf("11\n");
    }
    delete c_pack;
    delete pack_cls;
    for (int i = 0; i < real_num; i++)
    {
        delete pack_ret[i];
    }
    delete[] pack_ret;
    if (error_flag)
    {
        return false;
    }
    //xl.appendlog_recv(my_links[client].get_user_name(), "ClientTextAskForTexts");
    return true;
}
//Set
void UserManager::ClientUserSetUpdate(const int &client, const PacketHead& p,const char *b)
{
    if (!my_links[client].get_user_status())
    {
        return;
    }
    ClientToServerUserSetUpdate *c_pack;
    c_pack = new ClientToServerUserSetUpdate();
    c_pack->set_string(p,b);
    CreateRetSetPacket(c_pack, my_links[client].get_user_name());
   //xl.appendlog_userconfig_change(my_links[client].get_user_name());
    delete c_pack;
    return;
}
//Close
void UserManager::ClientClose(const int &client)
{
    vector<User>::iterator uit;
    vector<User>::iterator uz;
    auto fit=file_manage.find(string(my_links[client].get_user_name()));
    if(fit!=file_manage.end()){
        file_manage.erase(fit);
    }
    my_links[client].set_user_status(false);
    uit = find(my_clients.begin(), my_clients.end(), my_links[client].get_user_name());
    if (uit != my_clients.end())
    {
        if (client == uit->get_user_sockfd())
        {
            ServerToClientBase *pack_offline;
            pack_offline = CreateRetInformPacket(0, uit->get_user_name());
            uit->set_user_status(false);
            for(auto client:my_clients){
                if(client.get_user_status()){
                    my_links[client.get_user_sockfd()].SendMessage(pack_offline);
                }
            }
            delete pack_offline;
        }
    }
    //xl.appendlog_user_logout(my_links[client].get_user_name());
    close(client);
    return;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ADD~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//File online transfer
void UserManager::ClientFileOnlineNotify(const int& client,PacketHead& p,const char* b)
{
    if (!my_links[client].get_user_status())
    {
        return;
    }
    vector<User>::iterator uit;
    SenderToReceiverFileNotify* c_pack;
    int send_sock;
    c_pack = new SenderToReceiverFileNotify();
    c_pack->set_string(p,b);
    if(p.get_function_type()==PacketHead::kC2CFileNotifyRequest){
        FileNode fn(c_pack->get_file_name(),c_pack->get_file_size());
        file_manage[string(c_pack->get_sender_name())].push_back(fn);
    }else if(p.get_function_type()!=PacketHead::kC2CFileNotifyAccept){
        auto fit=file_manage.find(string(c_pack->get_sender_name()));
        if(fit!=file_manage.end()){
            auto eit=find(fit->second.begin(),fit->second.end(),c_pack->get_file_name());
            if(eit!=fit->second.end()){
                fit->second.erase(eit);
            }
        }
    }
    if(p.get_function_type()==PacketHead::kC2CFileNotifyRequest||p.get_function_type()==PacketHead::kC2CFileNotifyCancelSend){
        uit=find(my_clients.begin(),my_clients.end(),c_pack->get_receiver_name());
    }else{
        uit=find(my_clients.begin(),my_clients.end(),c_pack->get_sender_name());
    }
    if (uit != my_clients.end())
    {
        if (uit->get_user_status())
        {
            send_sock = uit->get_user_sockfd();
            my_links[send_sock].SendMessage(c_pack);
        }
        if (p.get_function_type() == PacketHead::kC2CFileNotifyRequest)
            xl.appendlog_onlinefilenotify(c_pack->get_sender_name(), c_pack->get_receiver_name(), c_pack->get_block_num(),"sender request for send file");
        else if (p.get_function_type() == PacketHead::kC2CFileNotifyCancelSend)
            xl.appendlog_onlinefilenotify(c_pack->get_sender_name(), c_pack->get_receiver_name(), c_pack->get_block_num(), "sender cancel to file");
        else if (p.get_function_type() == PacketHead::kC2CFileNotifyAccept)
            xl.appendlog_onlinefilenotify(c_pack->get_sender_name(), c_pack->get_receiver_name(), c_pack->get_block_num(), "receiver accept for recv file");
        else if (p.get_function_type() == PacketHead::kC2CFileNotifyCancelRecv)
            xl.appendlog_onlinefilenotify(c_pack->get_sender_name(), c_pack->get_receiver_name(), c_pack->get_block_num(), "receiver cancel to file");
    }
    delete c_pack;    
    return;
}
void UserManager::ClientFileOnlineData(const int& client,const PacketHead& p,const char* b)
{
    if (!my_links[client].get_user_status())
    {
        return ;
    }
    SenderToReceiverFileData* c_pack;
    vector<User>::iterator uit;
    int fid;
    int send_sock;
    c_pack = new SenderToReceiverFileData();
    c_pack->set_string(p,b);
    uit=find(my_clients.begin(),my_clients.end(),c_pack->get_receiver_name());
    if (uit != my_clients.end()){
        if (uit->get_user_status()){
            send_sock = uit->get_user_sockfd();
            my_links[send_sock].SendMessage(c_pack);
            //printf("13\n");
            xl.appendlog_onlinefile(c_pack->get_sender_name(), c_pack->get_receiver_name(), c_pack->get_block_num());
            if (c_pack->get_block_num() == 0xffff)
            {
                //文件传输完成
                char *now_time = new char[21];
                auto fit=file_manage.find(string(c_pack->get_sender_name()));
                int fsize=0;
                if(fit!=file_manage.end()){
                    auto vit=find(fit->second.begin(),fit->second.end(),c_pack->get_file_name());
                    if(vit!=fit->second.end()){
                        fsize=vit->file_size;
                        fit->second.erase(vit);
                    }
                }
                get_now_time(now_time);
                fid = mydb.PushBackFileContain(fsize, c_pack->get_file_name(), "");
                mydb.PushBackUserReceive(c_pack->get_sender_name(), now_time, c_pack->get_sender_name(),c_pack->get_receiver_name(), fid, 'f');
                mydb.PushBackUserReceive(c_pack->get_receiver_name(), now_time, c_pack->get_sender_name(),c_pack->get_receiver_name(), fid, 'f');
                delete[] now_time;
            }
        }
    }    
    delete c_pack;
    return;
}
//主要流程
void UserManager::ProvideService()
{

    int read_size = 0;
    char *buffer = new char[1024 * 1024 + 512];
    fd_set readfds, readfdsbak;
    PacketHead rec_head;
    FD_ZERO(&readfds);
    FD_SET(server_sock, &readfds);
    while (1)
    {
        readfdsbak = readfds;
        if (select(1024, &readfdsbak, NULL, NULL, NULL) < 0)
        {
            continue;
        }
        //printf("select ok\n");
        for (int i = 0; i < 1024; i++)
        {
            if (!FD_ISSET(i, &readfdsbak))
            {
                continue;
            }
            if (i == server_sock)
            {
                int clientsock;
                sockaddr_in echoclient;
                memset(&echoclient, 0, sizeof(echoclient));
                unsigned int clientlen = sizeof(echoclient);
                while ((clientsock = accept(server_sock, (struct sockaddr *)&echoclient, &clientlen)) < 0){
                    continue;
                }
                SocketFunction::SetSockNonBlock(clientsock);
                FD_SET(clientsock, &readfds);
                my_links[clientsock].Clear();
                my_links[clientsock].set_user_sockfd(clientsock);
                //xl.appendlog_others("link ok");
            }
            else
            {
                int close_flag = 0;
                read_size = SocketFunction::MyRecvC(i, 8, buffer);
                if (read_size == 0)
                {
                    close_flag = 1;
                }
                else
                {
                    rec_head.set_string(buffer);
                    read_size = SocketFunction::MyRecvC(i, rec_head.get_length(), buffer);
                    if (read_size == 0&& rec_head.get_length()!=0)
                    {
                        close_flag = 1;
                    }
                    else
                    {
                        switch (rec_head.get_packet_type())
                        {
                        case PacketHead::kC2SReport:
                        {
                            switch (rec_head.get_function_type())
                            {
                            case PacketHead::kC2SReportLoginIn:
                            {
                                close_flag = !ClientSign(i, rec_head, buffer);
                                //xl.appendlog_user_login(my_links[i].get_user_name());
                                break;
                            }
                            case PacketHead::kC2SReportRegister:
                            {
                                close_flag = !ClientRegister(i, rec_head, buffer);
                                //xl.appendlog_user_add(my_links[i].get_user_name());
                                break;
                            }
                            case PacketHead::kC2SReportUpdate:
                            {
                                close_flag = !ClientUpdate(i, rec_head, buffer);
                                //xl.appendlog_passwd_change(my_links[i].get_user_name());
                                break;
                            }
                            default:
                            {
                                //xl.appendlog_others("packethead = kC2SReport, while type not in login register and updata");
                                break;
                            }
                            }
                            break;
                        }
                        case PacketHead::kC2SText:
                        {
                            switch (rec_head.get_function_type())
                            {
                            case PacketHead::kC2STextToUsers:
                            {
                                close_flag = !ClientTextToUsers(i, rec_head, buffer);
                                //xl.appendlog_recv(my_links[i].get_user_name(), "kC2STextToUsers");
                                break;
                            }
                            case PacketHead::kC2STextFileToUsers:
                            {
                                close_flag = !ClientFileToUsers(i, rec_head, buffer);
                                //xl.appendlog_recv(my_links[i].get_user_name(), "kC2STextFileToUsers");
                                break;
                            }
                            case PacketHead::kC2STextAskForFile:
                            {
                                close_flag = !ClientFileToUsers(i, rec_head, buffer);
                                close_flag = !ClientTextAskForFile(i, rec_head, buffer);
                                //xl.appendlog_recv(my_links[i].get_user_name(), "kC2STextAskForFile");
                                break;
                            }
                            case PacketHead::kC2STextAskForTexts:
                            {
                                close_flag = !ClientTextAskForTexts(i, rec_head, buffer);
                                //xl.appendlog_recv(my_links[i].get_user_name(), "kC2STextAskForTexts");
                                break;
                            }
                            default:
                            {
                                break;
                            }
                            }
                            break;
                        }
                        case PacketHead::kC2SUserSet:
                        {
                            switch (rec_head.get_function_type())
                            {
                            case PacketHead::kC2SUserSetUpdate:
                            {
                                ClientUserSetUpdate(i, rec_head, buffer);
                                //xl.appendlog_recv(my_links[i].get_user_name(), "kC2SUserSetUpdate");
                                break;
                            }
                            default:
                            {
                                break;
                            }
                            }
                            break;
                        }
                        //TODO
                        case PacketHead::kC2CFileNotify:
                        {
                            switch (rec_head.get_function_type())
                            {
                            case PacketHead::kC2CFileNotifyRequest:
                            case PacketHead::kC2CFileNotifyCancelSend:
                            case PacketHead::kC2CFileNotifyAccept:
                            case PacketHead::kC2CFileNotifyCancelRecv:
                            {
                                ClientFileOnlineNotify(i,rec_head,buffer);
                                break;
                            }
                            default: 
                            {
                                break;
                            }
                            }
                            break;
                        }
                        case  PacketHead::kC2CFileData: 
                        {
                            switch (rec_head.get_function_type())
                            {
                            case PacketHead::kC2CFileDataSend:
                            {
                                ClientFileOnlineData(i,rec_head,buffer);
                                break;
                            }
                            default: 
                            {
                                break;
                            }
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                        }
                    }
                }
                if (close_flag)
                {
                    ClientClose(i);
                    FD_CLR(i, &readfds);
                }
            }
        }
    }
}

//
void creat_daemon(void)
{
    pid_t pid;
    pid = fork();

    signal(SIGCHLD, SIG_IGN);

    if (pid == -1)
        exit(0);
    if (pid > 0)
        exit(0);
    if (setsid() == -1)
        exit(0);
    //chdir("/");
    int i;
    umask(0);
    return;
}

//MAIN
int main()
{
    creat_daemon();
    UserManager myUserManager;
    myUserManager.ProvideService();
}

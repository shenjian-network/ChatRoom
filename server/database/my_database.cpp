#include "my_database.h"
const char MyDatabase::host[30]="localhost";
const char MyDatabase::user[30]="u1652310";
const char MyDatabase::passwd[30] = "u1652310"; //"" for lsx ; "u1652310" for sj
const char MyDatabase::charset[30]="utf8";
const char MyDatabase::database[30]="db1652310";

MyDatabase::MyDatabase()
{
    last_insert_id=0;
    Init();
}

void MyDatabase::MyDatabaseDie()
{
	printf( "mysql failed(%s)\n", mysql_error(conn));
	exit(1);
}

void MyDatabase::MyQuery(MYSQL * conn,const char* s)
{
	if (mysql_query(conn, s)) {
		MyDatabaseDie();
	} 
}

void MyDatabase::Init()
{
    // conn = mysql_init(NULL);
	// if (conn == NULL) {
	// 	MyDatabaseDie();
	// }
    // if (mysql_real_connect(conn, host, user, passwd, NULL, 0, NULL, 0) == NULL)
    // {
    //     MyDatabaseDie();
	// }
    // if (mysql_set_character_set(conn, "utf8")){
    //     MyDatabaseDie();
    // }
    // MyQuery(conn,"drop database if exists db1652310");
    // MyQuery(conn,"create database db1652310 charset=utf8 collate=utf8_bin");
    // MyQuery(conn,"use db1652310");
    // MyQuery(conn,"drop table if exists user_info");
    // MyQuery(conn,"drop table if exists inteflow_info");
    // MyQuery(conn,"drop table if exists text_contain");
    // MyQuery(conn,"drop table if exists file_contain");
    // MyQuery(conn,"create table user_info( "\
    //             "uname char(32) primary key,"\
    //             "pwd char(32) not null,"\
    //             "last_login_time datetime default '0000.00.00 00:00:00',"\
    //             "user_set mediumblob"\
    //         ")engine=innodb,charset=utf8,collate=utf8_bin");
    // MyQuery(conn,"create table zyc_msgtable("\
    //             "um_id int unsigned primary key auto_increment,"\
    //             "send_time datetime not null,"\
    //             "from_user char(32) not null," \
    //             "cid int unsigned," \
    //             "ctype char(1) default 'm'," \
    //             "foreign key(`from_user`) references user_info(`uname`)" \
    //         ")engine=innodb,charset=utf8,collate=utf8_bin");
    // MyQuery(conn,"create table lsx_msgtable( "\
    //             "um_id int unsigned primary key auto_increment," \
    //             "send_time datetime not null," \
    //             "from_user char(32) not null," \
    //             "cid int unsigned," \
    //             "ctype char(1) default 'm'," \
    //             "foreign key(`from_user`) references user_info(`uname`)" \
    //         ")engine=innodb,charset=utf8,collate=utf8_bin");
    // MyQuery(conn,"create table text_contain(" \
    //             "mid int unsigned primary key auto_increment," \
    //             "mcontain mediumtext" \
    //         ")engine=innodb,charset=utf8,collate=utf8_bin");
    // MyQuery(conn,"create table file_contain(" \
    //             "fid int unsigned primary key auto_increment, "\
    //             "fsize int," \
    //             "fname char(64)," \
    //             "fcontain mediumblob" \
    //         ")engine=innodb,charset=utf8,collate=utf8_bin");
    // MyQuery(conn,"insert into user_info values('zyc','123456','0000/00/00 00:00:00',NULL)");
    // MyQuery(conn,"insert into user_info values('lsx','123456','0000/00/00 00:00:00',NULL)");
	// mysql_close(conn);
}

int MyDatabase::MySelect(const int& rnum,const int& lnum,const char *q,char*** r,unsigned long** length_all)
{
    MYSQL_RES * result;
	MYSQL_ROW row;
    unsigned int field_num;
    unsigned long* lengths;
    int cur=0;
    MyQuery(conn, q);
    result = mysql_store_result(conn);
    field_num = mysql_num_fields(result);
    if(field_num!=lnum){
        return -1;
    }
    while((row = mysql_fetch_row(result))&&((++cur)<=rnum)){
        for(int i=0;i<field_num;i++){
            lengths = mysql_fetch_lengths(result);
            //printf("row:%d line:%d value:%lu\n",cur-1,i,lengths[i]);
            length_all[cur-1][i]=lengths[i];
            if(row[i]!=NULL){
                //printf("row:%d line:%d\n",cur-1,i);
                memcpy(r[cur-1][i],row[i],lengths[i]+1);
            }
        }
    }
    //printf("row num:%d\n",cur);
    return cur;
}
bool MyDatabase::MyNoSelect(const char *q)
{
    MYSQL_RES * result;
	MYSQL_ROW row;
    if(mysql_query(conn, q)){
        printf( "mysql NoSelect failed(%s)\n", mysql_error(conn));
        return false;
    }
    MyQuery(conn, "SELECT last_insert_id()");
    result = mysql_store_result(conn);
    row = mysql_fetch_row(result);
    last_insert_id=atoi(row[0]);
    return true;
}
bool MyDatabase::MyNoSelect(const int& l,const char *q)
{
    MYSQL_RES * result;
	MYSQL_ROW row;
    if(mysql_real_query(conn, q, l)){
        printf( "mysql NoSelect failed(%s)\n", mysql_error(conn));
        return false;
    }
    MyQuery(conn, "SELECT last_insert_id()");
    result = mysql_store_result(conn);
    row = mysql_fetch_row(result);
    last_insert_id=atoi(row[0]);
    return true;
}
unsigned int MyDatabase::get_last_insert_id()
{
    return last_insert_id;
}
MYSQL* MyDatabase::get_conn()
{
    conn = mysql_init(NULL);
	if (conn == NULL) {
		MyDatabaseDie();
	}
	if (mysql_real_connect(conn, host, user, passwd, database, 0, NULL, 0) == NULL) {
        MyDatabaseDie();
	}
    mysql_set_character_set(conn, "utf8");
    return conn;
}
void MyDatabase::close_conn()
{
    mysql_close(conn);
}

//TODO for test
// int main()
// {
//     printf("test:\n");
//     MyDatabase mydb;
//     MYSQL* conns;
//     int id;
    // char*** re;
    // unsigned long** l_all;
    // re=new char**[10];
    // l_all=new unsigned long*[10];
    // for(int i=0;i<10;i++)
    // {
    //     re[i]=new char*[4];
    //     l_all[i]=new unsigned long[4];
    //     for(int j=0;j<4;j++){
    //         re[i][j]=new char[100];
    //     }
    // }
    // char chunk_set[201];
    // char u_set[100]="12345";
    // conns=mydb.get_conn();
    // mysql_real_escape_string(conns, chunk_set, u_set, 5);
    // for(int i=0;i<7;i++){
    //     printf("%d\n",chunk_set[i]);
    // }
    //char format_q[255]="update `user_info` set `user_set`='%s' where `uname`='%s'";
    //char my_q[3*1024*1024];
    //query_len=snprintf(my_q,3*1024*1024,format_q,chunk_set,name);
    // int t=mydb.MySelect(3,4,"select * from user_info",(char***)re,(unsigned long**)l_all);
    // printf("show %d row\n",t);
    // for(int i=0;i<t;i++)
    // {
    //     for(int j=0;j<4;j++)
    //     {
    //         if(l_all[i][j]!=0)
    //             printf("%s:%lu \t",re[i][j],l_all[i][j]);
    //         else{
    //             printf("NULL\t");
    //         }
    //     }
    //     printf("\n");
    // }
    // //mydb.MyNoSelect("insert into zyc_msgtable values(NULL,'1998/10/21 00:00:00','zydc',NULL,NULL)");
    // mydb.MyNoSelect("insert into zyc_msgtable values(NULL,'1998/10/21 00:00:00','zyc',NULL,NULL)");
    // mydb.MyNoSelect("insert into zyc_msgtable values(NULL,'1998/10/21 00:00:00','zyc',NULL,NULL)");
    // printf("res:%d\n",mydb.get_last_insert_id());
    // mydb.MyNoSelect("insert into lsx_msgtable values(NULL,'1998/10/21 00:00:00','zyc',NULL,NULL)");
    // printf("res:%d\n",mydb.get_last_insert_id());
//     mydb.close_conn();
// }
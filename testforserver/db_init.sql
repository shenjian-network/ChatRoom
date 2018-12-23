set names utf8;
drop database if exists db1652310;
create database db1652310 charset=utf8 collate=utf8_bin;
use db1652310;
drop table if exists user_info;
drop table if exists inteflow_info;
drop table if exists text_contain;
drop table if exists file_contain;
create table user_info(
    uname char(32) primary key,
    pwd char(32) not null,
    last_login_time datetime default '0000.00.00 00:00:00',
    user_set mediumblob
)engine=innodb,charset=utf8,collate=utf8_bin;

create table zyc_msgtable(
    um_id int unsigned primary key auto_increment,
    send_time datetime not null,
    from_user char(32) not null,
    cid int unsigned,
    ctype char(1) default 'm',
    foreign key(`from_user`) references user_info(`uname`)
)engine=innodb,charset=utf8,collate=utf8_bin;

create table lsx_msgtable(
    um_id int unsigned primary key auto_increment,
    send_time datetime not null,
    from_user char(32) not null,
    cid int unsigned,
    ctype char(1) default 'm',
    foreign key(`from_user`) references user_info(`uname`)
)engine=innodb,charset=utf8,collate=utf8_bin;

create table text_contain(
    mid int unsigned primary key auto_increment,
    mcontain mediumtext
)engine=innodb,charset=utf8,collate=utf8_bin;

create table file_contain( 
    fid int unsigned primary key auto_increment, 
    fsize int, 
    fname char(64), 
    fcontain mediumblob 
)engine=innodb,charset=utf8,collate=utf8_bin;

#for test 
insert into user_info values('zyc','123456',NULL,NULL);
insert into user_info values('lsx','123456',NULL,NULL);

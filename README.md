# ChatRoom

**due 12.23 23:59:59**

简单群聊软件实现

采用**增量开发**



## 开发环境

### client
- C++ 
- QT

### server
- C++
- Mysql

## TODO

首先需要设计

- [ ] 报文格式
- [ ] 数据库表

### 基本要求

- [ ] 用户登录
- [ ] 用户改密
- [ ] 重复登录，后者踢掉前者
- [ ] 可以看到在线的其他用户
- [ ] 可以向某一个人发送数据
- [ ] 可以向全体发送数据
- [ ] 可以传送文件
- [ ] server需要记录log
- [ ] client信息尽量存放在server端

安全
- [ ] 数据库password字段需要加密

### Awards

- [ ] 可以传输jpg图片、git图片，要求能显示、有动画效果(3')
- [ ] client可以指定向哪些人发送消息(1')
- [ ] server的log采用xml格式(1')
- [ ] 修改用户设置(2')
- [ ] 采用OpenSSL(2')
- [ ] 完成手机端client(5')
- [ ] 其他特色功能可根据实际情况加分
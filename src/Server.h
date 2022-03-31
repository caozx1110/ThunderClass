/*************************************************************************
【文件名】Server.h
【功能模块和目的】服务端类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【版权信息】
    以下代码来自于对博客园jz_cnblogs用户的文章的修改和封装
    源文链接https://www.cnblogs.com/jzincnblogs/p/5189636.html
    范静涛已代表2020春季学期自动化系C++大作业目全体同学，获得原作者的版权授权，出于教学和学习目的，可做出任何修改
    范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对修改部分的任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和类结构优化
    2020-05-15 由范静涛增加注释
*************************************************************************/
#ifndef SERVERSOCKETCLASS_H
#define SERVERSOCKETCLASS_H

#include <Winsock2.h>
#include <windows.h>
#include <stdexcept>
#include <utility>
#include <vector>
#include <queue>
#include "Connection.h"
#include "Message.h"

using namespace std;

#define IP_BUF_SIZE 129

/*************************************************************************
【类名】Server
【功能】socket服务端类，用于监听客户端网络连接请求、连接、断开等操作
【接口说明】
    构造函数Server();
    析构函数~Server();
    函数Open(unsigned int Port =  DEFAULT_PORT)打开端口，等待客户端连入。Port默认值DEFAULT_PORT是类的静态成员
    函数Close()关闭端口，断开现有连接，并拒绝后续客户端连入
    函数GetAndRemoveFirstMessage(pair<unsigned int, Message>& aMessagePair)从消息队列里提取并删除第一条。如果有第一条的话，返回ture
    函数SendTo(unsigned int idx, const Message& aMessage)向idx指定的客户端发送一个消息，idx从0开始
    函数SendToAll(const Message& aMessage)群发消息
    函数IsConnected(unsigned int idx)返回指定Connection是否在线
    函数DisConnect(unsigned int idx)断开指定连接
    函数GetAvgToSendCount()返回全部正在工作的连接的待发送消息数量均值
    常数据成员IsOpened表示服务器是否打开端口进行监听
    静态常数据成员DEFAULT_PORT表示默认端口号
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和结构优化
    2020-05-15 由范静涛增加注释
*************************************************************************/
class Server
{
public:
    //构造函数
    Server();

    //析构函数
    ~Server();

    //不允许存在拷贝构造函数
    Server(const Server &) = delete;

    //不允许存在赋值运算符
    Server & operator=(const Server &) = delete;

    //打开端口，等待客户端连入。Port默认值DEFAULT_PORT是类的静态成员
    void Open(unsigned int Port =  DEFAULT_PORT);

    //关闭端口，断开现有连接，并拒绝后续客户端连入
    void Close();

    //从消息队列里提取并删除第一条。如果有第一条的话，返回ture
    bool GetAndRemoveFirstMessage(pair<unsigned int, Message>& aMessagePair);

    //向idx指定的客户端发送一个消息，idx从0开始
    bool SendTo(unsigned int idx, const Message& aMessage);

    //群发消息
    void SendToAll(const Message& aMessage);

    //指定连接是否在线
    bool IsConnected(unsigned int idx);

    //断开指定连接
    void DisConnect(unsigned int idx);

    //获得全部活动连接的待发送消息数量均值
    unsigned int GetAvgToSendCount();

    //端口是否打开。与Connection类的IsConnected()功能相同，但使用更为方便，强烈推荐这种形式
    const bool& IsOpend;

    //默认端口号
    static unsigned int  DEFAULT_PORT;
private:

    //看不懂，也不用看懂系列开始
    WORD m_winsock_ver;
    WSADATA m_wsa_data;
    SOCKET m_SocketServer;
    SOCKADDR_IN addr_svr;
    SOCKADDR_IN addr_clt;
    int ret_val;
    int addr_len;
    char buf_ip[IP_BUF_SIZE];
    //看不懂也不用看懂系列结束，都是从原始代码上照搬的
    
    //端口号
    unsigned int m_uPort;

    //用于存放已经连接进来的全部客户端的Connection对象指针
    vector<Connection*> m_Connections;

    //监听线程句柄
    vector<HANDLE> m_ThreadHadle;

    //接收消息的队列
    queue<pair<unsigned int, Message> > m_Messages;

    //等待(监听)客户端连接请求的线程函数。要么是static，要么是friend，具体理由可不掌握，特别感兴趣的同学可以联系范老师哦
    static DWORD WINAPI WaitThread(LPVOID lpParameter);

    //是否处于等待(监听)状态，Open和Close函数会改变此私有成员，公有成员const bool& IsOpend就是引用的此私有成员
    bool m_bIsListening;
};

#endif

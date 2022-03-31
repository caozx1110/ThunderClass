/*************************************************************************
【文件名】Connection.h
【功能模块和目的】用于服务端的，与客户端的连接类型声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【版权信息】
    以下代码来自于对博客园jz_cnblogs用户的文章的修改和封装
    源文链接https://www.cnblogs.com/jzincnblogs/p/5189636.html
    范静涛已代表2020春季学期自动化系C++大作业目全体同学，获得原作者的版权授权，出于教学和学习目的，可做出任何修改
    范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对修改部分的任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和类结构优化
    2020-05-15 由范静涛增加注释
    2020-05-23 由范静涛增加了收发消息队列加锁互斥访问机制，感谢李永健和代祥齐发现问题并提供log文件
*************************************************************************/
#ifndef CONNECTION_H
#define CONNECTION_H

#include "Constant.h"
#include <Winsock2.h>
#include <windows.h>
#include <vector>
#include <utility>
#include <queue>
//2020-05-23 新增C++标准头文件
#include <mutex>
//2020-05-23 新增C++标准头文件 结束
#include "Message.h"
using namespace std;

#define IP_BUF_SIZE 129

/*************************************************************************
【类名】Connection
【功能】用于服务端的，与客户端收发消息
【接口说明】
    构造函数Connection
    析构函数~Connection
    函数Disconnect断开连接
    函数Send发送一个消息
    函数IsConnected返回连接状态
    函数ToSendCount返回待发队列长度
    静态数据成员MSG_BUF_SIZE表示每次最多接收多少字节
    静态数据成员RecvMsgMutex, 2020-05-23 由范静涛增加
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和结构优化
    2020-05-15 由范静涛增加注释
    2020-05-23 由范静涛增加了收消息队列加锁互斥访问机制
*************************************************************************/
class Connection{
public:
    //构造函数
    //第1参数，来自于Server提供的、已经连上的SocketClient（不需深入理解，它就是一个连接的标识，和身份证一样）
    //第2参数，2020-05-10范静涛添加，表示此Connection是Server创建第几个，最早创建的是0号Connections对象
    //第3参数，2020-05-10范静涛修改，pair是stl中的一种类型，first表示当前Connection的index，second表示消息。此参数是一个pair的队列的引用，其本期来自Server
    Connection(SOCKET SocketClient, unsigned int Index, queue<pair<unsigned int, Message> >& OwnerMessages);

    //析构函数
    ~Connection();

    //不允许存在拷贝构造函数
    Connection(const Connection&) = delete;

    //不允许存在赋值运算符
    Connection& operator= (const Connection&) = delete;

    //用于断开与客户端的链接
    void Disconnect();

    //用于向客户端发送一个消息
    bool Send(const Message& Message);

    //返回值表示是否与客户端处于连接状态。断开连接后，此Connection对象仍然存在，直到被销毁。但断开后，发送消息的功能就不可用了
    bool IsConnected();

    //返回还有多少个消息没发送出去，如已断开连接返回0
    unsigned int ToSendCount();

    //消息接收缓冲区域的大小。static const限定，是因为这个属性是类的属性，应在全体对象上保持一致，且不可改变
    static const unsigned int MSG_BUF_SIZE;

    //2020-05-23 由范静涛增加, 用于实现收消息队列的有序插入和删除
    static mutex RecvMsgMutex;
private:
    //来自于Server提供的、已经连上的SocketClient（不需深入理解，它就是一个连接的标识，和身份证一样），保存在此私有数据成员
    SOCKET SocketClient;

    //表示是否处于与客户端的连接状态，做成私有是为了外部仅能通公有IsConnected函数读到，但不可改变此成员的值。体现面型对象的封装概念，隐藏内部机制
    bool m_bIsConnected;

    //此Connection对象在Server的Connections里的下标
    unsigned int m_uIndex;

    //要发送给客户端的信息队列
    queue<Message> m_ToSend;

    //服务器的(接收)消息的队列的引用，向此成员添加消息，实际是添加到了服务器的消息队列中
    queue<pair<unsigned int, Message> >& m_Messages;

    //与客户端实际收发消息的线程函数。要么是static，要么是friend，具体理由可不掌握，特别感兴趣的同学可以联系范老师哦
    static DWORD WINAPI ConnectionThread(LPVOID lpParameter);

    //收发消息句柄
    HANDLE m_hThread;

    //2020-05-23 由范静涛增加, 用于实现发消息队列的有序插入和删除
    mutex m_SendMsgMutex;

};

#endif // CONNECTION_H

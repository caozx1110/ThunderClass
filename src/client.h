/*************************************************************************
【文件名】client.h
【功能模块和目的】socket客户端类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【版权信息】
    以下代码来自于对博客园jz_cnblogs用户的文章的修改和封装
    源文链接https://www.cnblogs.com/jzincnblogs/p/5170230.html
    范静涛已代表2020春季学期自动化系C++大作业目全体同学，获得原作者的版权授权，出于教学和学习目的，可做出任何修改
    范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对修改部分的任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和类结构优化
    2020-05-15 由范静涛增加注释
    2020-05-23 由范静涛增加了收发消息队列加锁互斥访问机制，感谢李永健和代祥齐发现问题并提供log文件
*************************************************************************/

#ifndef CLIENT_H
#define CLIENT_H

#include "Constant.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#include <stdexcept>
#include "Message.h"
#include <queue>
//2020-05-23 新增头文件
#include <mutex>
using namespace std;

//使用vs环境的时候，请取消下行注释
//#pragma comment(lib, "ws2_32.lib")

/*************************************************************************
【类名】Client
【功能】socket客户端类，用于连接到服务端并收发通用消息
【接口说明】
    构造函数Client();
    析构函数~Client();
    函数ConnectTo连接到指定IP和Port所代表的服务端
    函数DisConnect断开连接
    函数Send发送一个消息
    函数GetAndRemoveFirstRecvMsg获取并移除最早收到的消息
    函数Clear清空收发消息队列
    常引用IsConnected表示连接状态
    静态常数据成员DEFAULT_PORT表示默认端口号
    静态常数据成员MSG_BUF_SIZE表示每次最多收多少字节
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
class Client
{
public:
    //构造函数
    Client();

    //析构函数
    ~Client();

    //禁用拷贝构造函数
    Client(const Client&) = delete;

    //禁用赋值运算符
    Client& operator=(const Client&) = delete;

    //连接到指定IP和Port
    void ConnectTo(const string& IP, unsigned Port = DEFAULT_PORT);

    //断开连接
    void DisConnect();

    //发送一个消息
    bool Send(const Message& aMessage);

    //获取并移除最早收到的消息
    bool GetAndRemoveFirstRecvMsg(Message& aMessage);

    //清空收发消息队列
    void Clear();

    //连接状态
    const bool& IsConnected;

    //默认端口号
    static const unsigned int DEFAULT_PORT;

    //默认消息缓冲区大小，只在接收时有用
    static const unsigned int MSG_BUF_SIZE;
private:
    //通信的线程函数
    static DWORD WINAPI CommunicationThread(LPVOID lpParameter);

    //通信线程句柄
    HANDLE m_hCommThread;

    //Server端Socket
    SOCKET m_SocketServer;

    //连接状态
    bool m_bIsConnected;

    //收到的消息队列
    queue<Message> m_Messages;

    //待发的消息队列
    queue<Message> m_ToSend;

    //2020-05-23 由范静涛新增 收消息互斥锁
    mutex m_RecvMsgMutex;

    //2020-05-23 由范静涛新增 发消息互斥锁
    mutex m_SendMsgMutex;
};

#endif // CLIENT_H

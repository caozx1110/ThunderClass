/*************************************************************************
【文件名】Connection.cpp
【功能模块和目的】用于服务端的，与客户端的连接类型定义
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

#include "Connection.h"
#include <WS2tcpip.h>
#include <stdexcept>
using namespace std;

//Connection类的静态常成员，目标是每个Connection对象收消息时的缓冲区大小是固定不变的
const unsigned int Connection::MSG_BUF_SIZE = 1024;

//2020-05-23 由范静涛增加了收消息队列互斥公有静态数据成员RecvMsgMutex，实现Server和全部Connection对象的有序访问
mutex Connection::RecvMsgMutex;
//2020-05-23 RecvMsgMutex 增加结束

/*************************************************************************
【函数名称】Connection::Connection
【函数功能】构造函数
【参数】
     第1参数，来自于Server提供的、已经连上的SocketClient（不需深入理解，它就是一个连接的标识，和身份证一样）
     第2参数，2020-05-10范静涛添加，表示此Connection是Server创建第几个，最早创建的是0号Connections对象
     第3参数，2020-05-10范静涛修改，pair是stl中的一种类型，first表示当前Connection的index，second表示消息。此参数是一个pair的队列的引用，其本体来自Server
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Connection::Connection(SOCKET SocketClient, unsigned int Index, queue<pair<unsigned int, Message> >& OwnerMessages):m_Messages(OwnerMessages){
    //用形参初始化私有数据成员
    this->SocketClient = SocketClient;
    //当前连接处于不可用状态，因为需要有个线程与连接对应，如果后续线程创建失败呢？所以当前认为是false
    m_bIsConnected = false;
    //标记当前连接在Server全部连接中的下标
    m_uIndex = Index;
    //抄原始代码，只改动过参数的一行代码，目标是创建一个用于收发消息的线程
    //实参ConnectionThread是真正用于与客户端收发消息的函数
    //实参（LPOVID）this，是ConnectionThread运行时收到的唯一参数。也就是说，线程运行起来，唯一参数就代表它是属于哪一个具体Connection对象的
    m_hThread = CreateThread(nullptr, 0, ConnectionThread, (LPVOID)this, 0, nullptr);
    //如果现场创建不成功
    if (m_hThread == NULL)
    {
       //抛出一个异常
       throw(runtime_error("Failed to create a new thread!Error code: "
            + to_string(WSAGetLastError())));
    }
}

/*************************************************************************
【函数名称】Connection::~Connection
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Connection::~Connection() {
    //主动断开与客户端连接
    Disconnect();
}

/*************************************************************************
【函数名称】Connection::Disconnect
【函数功能】断开连接
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和结构优化
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Connection::Disconnect() {
    //收发线程在中断与客户端的socket连接时，会修改m_bIsConnected为false
    if (m_bIsConnected == false) {
        //未清理线程句柄
        if(m_hThread != nullptr) {
            CloseHandle(m_hThread);
            m_hThread = nullptr;
        }
        return;
    }
    //通信线程还在运行
    else {
        //更新为非通信状态
        m_bIsConnected = false;
        //等待线程结束
        WaitForSingleObject(m_hThread, WAITMS);
        //关闭并清理线程句柄
        CloseHandle(m_hThread);
        m_hThread = nullptr;
    }
    //清空待发消息队列
    m_ToSend = queue<Message>();
}

/*************************************************************************
【函数名称】Connection::Send
【函数功能】发送消息
【参数】aMessage,入口参数，表示要发送的通用消息
【返回值】ture表示已放入待发队列尾，false表示放入失败
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数形式
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Connection::Send(const Message& aMessage){
    //只有在正与客户端连接中，才会执行实际的发送操作
    if (m_bIsConnected) {
        //ToSend是一个待发送消息的队列，插进去即可。因为ConnectionThread会一直检查这个队列里是否有消息，有就发送
        //2020-05-23 由范静涛增加了lock和unlock代码
        m_SendMsgMutex.lock();
        m_ToSend.push(aMessage);
        m_SendMsgMutex.unlock();
        //2020-05-23 lock和unlock代码 增加结束
        return true;
    }
    return false;
}

/*************************************************************************
【函数名称】Connection::IsConnected
【函数功能】获取连接状态
【参数】无
【返回值】ture已连接，false表示已断开
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Connection::IsConnected(){
    return m_bIsConnected;
}

/*************************************************************************
【函数名称】Connection::ToSendCount
【函数功能】获取还有多少个没发送出去的消息
【参数】无
【返回值】没发送出去的消息的数量
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】
*************************************************************************/
unsigned int Connection::ToSendCount(){
    if (!m_bIsConnected) {
        return 0;
    }
    else {
        return m_ToSend.size();
    }
}

/*************************************************************************
【函数名称】Connection::ConnectionThread
【函数功能】通信线程函数
【参数】aMessage,入口参数，表示要发送的通用消息
【返回值】ture表示已放入待发队列尾，false表示放入失败
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数形式
    2020-05-15 由范静涛增加注释
*************************************************************************/
DWORD WINAPI Connection::ConnectionThread(LPVOID lpParameter){
    //实参是Connection*，所以可以强制类型转换，代表当前线程所属的connection
    Connection* Cnct = (Connection*)lpParameter;
    //更新状态为已连接
    Cnct->m_bIsConnected = true;
    //局部字符数组，长度为MSG_BUF_SIZE。用于存储接收的数据
    char buf_msg[MSG_BUF_SIZE];
    //每次接收的字节数，0表示断开了，小于0表示出错
    int RecvCount = 0;
    //发送的字节数，小于等于0表示断开或出错
    int SendCount = 0;

    //空闲消息，用于维持消息先收后发、一收一发的循环，只要不出错、客户端没断开，就会一直转啊转~~~~
    Message MsgIDLE = Message::MakeIdleMsg();
    //退出消息，用于通知业务流程类已经断开
    Message MsgEXIT = Message::MakeExitMsg();
    //临时消息，用于存储接收到的数据
    Message* Msg = new Message();

    //只要处于连接状态，就始终执行循环
    while (Cnct->m_bIsConnected) {
        //Step1,接收数据，最长MSG_BUF_SIZE，到buf_msg
        RecvCount = ::recv(Cnct->SocketClient, buf_msg, MSG_BUF_SIZE, 0);

        //Step2, 判断接收是否出错。RecvCount <= 0,代表接收出错或已对方已断开
        if (RecvCount <= 0) {
            //跳出循环就断开连接
            //跳出循环前，向Server消息队列中插入一个pair<当前Connection的Imdex，退出消息>
            //2020-05-23 新增以下的lock和unlock代码
            RecvMsgMutex.lock();
            Cnct->m_Messages.push(pair<unsigned int, Message>(Cnct->m_uIndex, MsgEXIT));
            RecvMsgMutex.unlock();
            //2020-05-23 lock和unlock 新增结束
            break;
        }

        //Step3,转换成消息
        unsigned int ToAppendCount = RecvCount;
        unsigned int RemInBufCount = 0;
        bool IsMsgValid = false;
        do {
            //向Msg追加接收到的数据，返回true表示追加的数据与之前的数据构成了一个完整消息；如果追加的多了，RemInBufCount表示在完整消息后还有多少字节没有处理
            IsMsgValid = Msg->Append((const unsigned char*)buf_msg, ToAppendCount, RemInBufCount);
            // 如果IsMessageFinished为false，RemInBufCount一定为0;
            if (IsMsgValid) {
                //添加完整消息到接收消息的队列
                //2020-05-23 新增以下的lock和unlock代码
                RecvMsgMutex.lock();
                Cnct->m_Messages.push(pair<unsigned int, Message>(Cnct->m_uIndex, *Msg));
                RecvMsgMutex.unlock();
                //2020-05-23 lock和unlock 新增结束
                //换个新消息等待下次使用
                delete Msg;
                Msg = new Message();
            }
            //前移未追加的数据到buf_msg最开头
            if (RemInBufCount != 0) {
                memmove(buf_msg, buf_msg + ToAppendCount - RemInBufCount, RemInBufCount);
            }
            //下次要追加的长度 = 当前剩余在buf_msg中的长度
            ToAppendCount = RemInBufCount;
        }while(ToAppendCount != 0); //要追加的长度不为0，则再追加

        //Step4 待发队列有消息就发，没有就发个空闲消息，维持一收一发机制
        //2020-05-23 由范静涛增加了lock和unlock代码
        Cnct->m_SendMsgMutex.lock();
        if (!Cnct->m_ToSend.empty()){
            //注意，ToSend是通用消息队列，不关注消息里是什么类型的内容
            //不管消息多大，都一次性发出去
            SendCount = send(Cnct->SocketClient, (const char*)Cnct->m_ToSend.front().Data, Cnct->m_ToSend.front().Size, 0);
            Cnct->m_ToSend.pop();
        }
        else{
            //发送空闲消息
            SendCount = send(Cnct->SocketClient, (const char*)MsgIDLE.Data, MsgIDLE.Size, 0);
        }
        Cnct->m_SendMsgMutex.unlock();
        //2020-05-23 lock和unlock代码 增加结束

        //step5 检查发送是否成功
        if (SendCount <= 0) {
            //跳出循环就断开连接
            //跳出循环前，向Server消息队列中插入一个pair<当前Connection的Imdex，退出消息>
            //2020-05-23 新增以下的lock和unlock代码
            RecvMsgMutex.lock();
            Cnct->m_Messages.push(pair<unsigned int, Message>(Cnct->m_uIndex, MsgEXIT));
            RecvMsgMutex.unlock();
            //2020-05-23 lock和unlock 新增结束
            break;
        }
    };
    //释放临时消息
    delete Msg;
    //关闭socket
    shutdown(Cnct->SocketClient, SD_BOTH);
    closesocket(Cnct->SocketClient);
    //更新为未连接状态
    Cnct->m_bIsConnected = false;
    return 0;
}

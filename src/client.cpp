/*************************************************************************
【文件名】client.cpp
【功能模块和目的】socket客户端类实现
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

#include "client.h"

//用于连接服务器的默认端口号
const unsigned int Client::DEFAULT_PORT = 4000;
//默认接收缓冲区大小，仅在接收数据是有用
const unsigned int Client::MSG_BUF_SIZE = 1024;

/*************************************************************************
【函数名称】Client::Client
【函数功能】构造函数，仅用于初始化通信线程句柄，socket对象，和当前连接状态
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Client::Client():IsConnected(m_bIsConnected){
    m_hCommThread = nullptr;
    m_SocketServer = INVALID_SOCKET;;
    m_bIsConnected = false;
}

/*************************************************************************
【函数名称】Client::~Client
【函数功能】析构函数，断开可能存在的连接，清除收发消息队列
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05107 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Client::~Client(){
    DisConnect();
    Clear();
}

/*************************************************************************
【函数名称】Client::ConnectTo
【函数功能】连接至特定IP地址和端口
【参数】IP，入口参数，代表服务端IP地址的字符串类型；Port，入口参数，代表端口号的无符号整型
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Client::ConnectTo(const string& IP, unsigned int Port){
    //已连接则直接返回，不执行任何实际动作
    if (m_bIsConnected) {
        return;
    }

    //以下基本忠于原文代码流程，仅修改了cerr输出错误为抛出异常
    WSADATA wsa_data; //WSADATA变量,包含windows socket执行的信息
    int i_result = 0; //接收返回值
    m_SocketServer = INVALID_SOCKET;
    addrinfo *result = nullptr, hints;
    //初始化winsock动态库(ws2_32.dll),MAKEWORD(2, 2)用于请求使用winsock2.2版本
    i_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (i_result != 0) {
        throw(runtime_error("WSAStartup() function failed: " + to_string(i_result)));
    }
    SecureZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    //此处对原文代码有修改，目的是通过给定参数连接到服务端
    i_result = getaddrinfo(IP.c_str(), to_string(Port).c_str(), &hints, &result);
    if (i_result != 0) {
        WSACleanup();
        throw(runtime_error("getaddrinfo() function failed with error: "
            + to_string(WSAGetLastError())));
    }
    //创建套接字
    m_SocketServer = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (m_SocketServer == INVALID_SOCKET) {
        WSACleanup();
        throw(runtime_error("socket() function failed with error: "
            + to_string(WSAGetLastError())));
    }
    //连接服务器
    i_result = connect(m_SocketServer, result->ai_addr, result->ai_addrlen);
    if (i_result == SOCKET_ERROR) {
        WSACleanup();
        return;
    }
    freeaddrinfo(result);
    //创建通信线程，如失败则抛出异常
    m_hCommThread = CreateThread(nullptr, 0, CommunicationThread, (LPVOID)this, 0, nullptr);
    if (m_hCommThread == NULL) {
       WSACleanup();
       throw(runtime_error("Failed to create a new thread!Error code: "
            + to_string(WSAGetLastError())));
    }
}

/*************************************************************************
【函数名称】Client::DisConnect
【函数功能】断开连接
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Client::DisConnect(){
    //通信线程在中断与服务端的socket连接时，会修改m_IsConnected为false
    if (m_bIsConnected == false) {
        //未清理线程句柄
        if(m_hCommThread != nullptr) {
            CloseHandle(m_hCommThread);
            m_hCommThread = nullptr;
        }
        return;
    }
    //通信线程还在运行
    else {
        //更新为非通信状态
        m_bIsConnected = false;
        //等待线程结束
        WaitForSingleObject(m_hCommThread, WAITMS);
        //关闭并清理线程句柄
        CloseHandle(m_hCommThread);
        m_hCommThread = nullptr;
    }
}

/*************************************************************************
【函数名称】Client::Send
【函数功能】发送消息
【参数】aMessage,入口参数，代表要发送的通用消息
【返回值】true代表已经将消息加入待发送队列
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Client::Send(const Message& aMessage){
    //当通信线程处于连接状态时，加入通用消息到待发送队列
    if (m_bIsConnected) {
        m_ToSend.push(aMessage);
        return true;
    }
    return false;
}

/*************************************************************************
【函数名称】Client::GetAndRemoveFirstRecvMsg
【函数功能】获取并删除接收队列中最早收到的消息
【参数】aMessage,出口参数，如接收队列中有消息，则用最早收到的消息向此参数赋值
【返回值】true代表获取到了最早的消息，false表示接收队列为空
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Client::GetAndRemoveFirstRecvMsg(Message& aMessage){
    //2020-05-23 由范静涛新增lcok代码
    m_RecvMsgMutex.lock();
    if (!m_Messages.empty()){
        aMessage = m_Messages.front();
        m_Messages.pop();
        //2020-05-23 由范静涛新增unlcok代码
        m_RecvMsgMutex.unlock();
        return true;
    }
    //2020-05-23 由范静涛新增unlcok代码
    m_RecvMsgMutex.unlock();
    return false;
}

/*************************************************************************
【函数名称】Client::Clear
【函数功能】清空收发队列
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Client::Clear(){
    //2020-05-23 由范静涛新增lcok代码
    m_RecvMsgMutex.lock();
    m_Messages = queue<Message>();
    //2020-05-23 由范静涛新增unlcok代码
    m_RecvMsgMutex.unlock();

    //2020-05-23 由范静涛新增lcok代码
    m_SendMsgMutex.lock();
    m_ToSend = queue<Message>();
    //2020-05-23 由范静涛新增unlcok代码
    m_SendMsgMutex.unlock();
}

/*************************************************************************
【函数名称】Client::CommunicationThread
【函数功能】通信线程函数
【参数】lpParameter，入口参数，代表线程所属的Client
【返回值】无意义，仅函数形式需要
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
DWORD WINAPI Client::CommunicationThread(LPVOID lpParameter){
    //实参是Client对象指针，所以可以强制类型转换
    Client* Clnt = (Client*)lpParameter;
    //线程启动后修改连接状态为true
    Clnt->m_bIsConnected = true;
    //接收数据缓冲区
    char buf_msg[MSG_BUF_SIZE];
    //每次实际接收到的字节数
    int RecvCount = 0;
    //每次实际发送的字节数
    int SendCount = 0;

    //维持客户端先发后收循环，定义一个空闲消息，只要不出错、客户端没断开，就会一直转啊转~~~~
    Message MsgIDLE = Message::MakeIdleMsg();
    //断开消息，向业务流程类通知已经断开与服务端连接
    Message MsgEXIT = Message::MakeExitMsg();
    //用于接收数据的消息
    Message* Msg = new Message();

    //只要处于连接状态，就始终玄幻
    while (Clnt->IsConnected) {
        //Step1 待发队列有消息就发，没有就发空闲
        //2020-05-23 由范静涛新增lcok代码
        Clnt->m_SendMsgMutex.lock();
        if (!Clnt->m_ToSend.empty()){
            //注意，ToSend是通用消息队列，不关注消息里是什么类型的内容
            //不管消息多大，都一次性发出去
            SendCount = send(Clnt->m_SocketServer, (const char*)Clnt->m_ToSend.front().Data, Clnt->m_ToSend.front().Size, 0);
            Clnt->m_ToSend.pop();
        }
        else{
            //待发队列为空，就发个空闲消息出去，维持发一个才能收一个的固定化通信流程
            SendCount = send(Clnt->m_SocketServer, (const char*)MsgIDLE.Data, MsgIDLE.Size, 0);
        }
        //2020-05-23 由范静涛新增unlcok代码
        Clnt->m_SendMsgMutex.unlock();

        //step2 检查发送是否成功
        if (SendCount == SOCKET_ERROR) {
            //跳出循环就断开连接
            //跳出循环前，向收到的消息队列压入一个通信退出的消息
            //2020-05-23 由范静涛新增lcok代码
            Clnt->m_RecvMsgMutex.lock();
            Clnt->m_Messages.push(MsgEXIT);
            //2020-05-23 由范静涛新增unlcok代码
            Clnt->m_RecvMsgMutex.unlock();
            break;
        }

        //Step3,接收数据，最长MSG_BUF_SIZE，到buf_msg
        RecvCount = ::recv(Clnt->m_SocketServer, buf_msg, MSG_BUF_SIZE, 0);

        //Step4, 判断接收是否出错。RecvCount <= 0,代表接收出错或已对方已断开
        if (RecvCount <= 0) {
            //跳出循环就断开连接
            //跳出循环前，向收到的消息队列压入一个通信退出的消息
            //2020-05-23 由范静涛新增lcok代码
            Clnt->m_RecvMsgMutex.lock();
            Clnt->m_Messages.push(MsgEXIT);
            //2020-05-23 由范静涛新增unlcok代码
            Clnt->m_RecvMsgMutex.unlock();
            break;
        }

        //Step5,将收到的数据转换成消息，与Message类函数配合，实现消息的分割，消除粘连
        unsigned int ToAppendCount = RecvCount;
        unsigned int RemInBufCount = 0;
        bool IsMsgValid = false;
        do {
            //向Msg追加接收到的数据，返回true表示追加的数据与之前的数据构成了一个完整消息；如果追加的多了，RemInBufCount表示在完整消息后还有多少字节没有处理
            IsMsgValid = Msg->Append((const unsigned char*)buf_msg, ToAppendCount, RemInBufCount);
            // 如果IsMessageFinished为false，RemInBufCount一定为0;
            if (IsMsgValid) {
                //2020-05-23 由范静涛新增lcok代码
                Clnt->m_RecvMsgMutex.lock();
                //添加完整消息到接收消息的队列
                Clnt->m_Messages.push(*Msg);
                //2020-05-23 由范静涛新增unlcok代码
                Clnt->m_RecvMsgMutex.unlock();
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
    };
    //删除临时用的消息对象
    delete Msg;
    //关闭socket
    shutdown(Clnt->m_SocketServer, SD_BOTH);
    closesocket(Clnt->m_SocketServer);
    WSACleanup();
    //更新为非连接状态
    Clnt->m_bIsConnected = false;
    return 0;
}

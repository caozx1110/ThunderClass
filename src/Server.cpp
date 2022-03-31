/*************************************************************************
【文件名】Server.cpp
【功能模块和目的】服务端类定义
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

#include "Server.h"

//Server类的静态成员，目标是在Open函数不指定端口号的时候就默认打开4000端口。因为Open函数的参数默认值在h文件中，它只能用常数或者静态成员做默认值
unsigned int Server:: DEFAULT_PORT = 4000;

/*************************************************************************
【函数名称】Server::Serve
【函数功能】构造函数
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释

*************************************************************************/
Server::Server():IsOpend(m_bIsListening),m_uPort(DEFAULT_PORT),m_bIsListening(false) {
    //服务器类的构造函数，使用初始化列表初始化
    //m_uPort是表示端口的私有成员
    //DEFAULT_PORTt是类的默认端口号，Server类的静态成员
    //IsListening是私有成员，表示是否正在等待（监听）客户端连接
    //IsOpend是公有常引用，和IsListening一个意思。纯粹的C++知识点展示
}


/*************************************************************************
【函数名称】Server::~Serve
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Server::~Server()
{
    //关闭已打开的端口，断开全部连接
    Close();
}


/*************************************************************************
【函数名称】Server::GetAndRemoveFirstMessage
【函数功能】从全部收到的消息中，拿出第一个未访问过的，并删除
【参数】参数，2020-05-10范静涛修改，pair是stl中的一种类型，first表示当前Connection的index，second表示消息。
【返回值】ture为队列中有未访问的消息
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Server::GetAndRemoveFirstMessage(pair<unsigned int, Message> & aMessagePair) {
    //消息队列不为空的话
    //2020-05-23 由范静涛新增了lock
    Connection::RecvMsgMutex.lock();
    if (!m_Messages.empty()) {
        //用第一条消息赋值给Message
        aMessagePair = m_Messages.front();
        //删除第一条消息
        m_Messages.pop();
        //2020-05-23 由范静涛新增了unlock
        Connection::RecvMsgMutex.unlock();
        //返回true表示Message是有效的
        return true;
    }
    //消息队列为空
    else {
        //2020-05-23 由范静涛新增了unlock
        Connection::RecvMsgMutex.unlock();
        //返回false表示Message是无效的
        return false;
    }            
}

/*************************************************************************
【函数名称】Server::SendTo
【函数功能】向指定客户端连接发送消息
【参数】均为入口参数，一次为客户端对应的connection连接号，要发送的消息
【返回值】ture为指定连接处于通信状态，可以发送
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Server::SendTo(unsigned int idx, const Message& aMessage) {
    //如果不存在idx指定的客户端，或者指定客户端处于断开的状态（Connection对象的IsActive是false）
    if (idx >= m_Connections.size() || !m_Connections[idx]->IsConnected()) {
        //返回false表示不能发送
        return false;
    }
    //存在指定的客户端，且其正在通信中（Connection对象的IsActive是false）
    else {
        //向指定客户端发送消息（由Connection类提供的Send函数）
        m_Connections[idx]->Send(aMessage);
        //返回ture表示可以发送成功
        return true;
    }
}

/*************************************************************************
【函数名称】Server::SendToAll
【函数功能】群发消息
【参数】入口参数，要发送的消息
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Server::SendToAll(const Message& aMessage){
    //遍历每个连接，依次发送出去
    for (unsigned int i  = 0; i < m_Connections.size(); i++) {
        //SendTo涵盖了客户端已断开的处理，这里并不关注SendTo的返回值。有多少个可以发的客户端，就发多少个消息
        SendTo(i, aMessage);
    }
}

/*************************************************************************
【函数名称】Server::Open
【函数功能】打开端口监听，开始等待客户端连接
【参数】入口参数，要打开的端口号
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Server::Open(unsigned int Port) {
    //如果端口已经打开了，就直接返回。重复开一个端口，会产生错误的
    if (m_bIsListening) {
        return;
    }
    //端口号赋值给私有成员
    m_uPort = Port;
    //以下不需要看懂，我也没看过，原始代码如此，能work。
    m_winsock_ver = MAKEWORD(2, 2);
    addr_len = sizeof(SOCKADDR_IN);
    addr_svr.sin_family = AF_INET;
    addr_svr.sin_port = ::htons(m_uPort);
    addr_svr.sin_addr.S_un.S_addr = ADDR_ANY;
    memset(buf_ip, 0, IP_BUF_SIZE);
    //
    ret_val = ::WSAStartup(m_winsock_ver, &m_wsa_data);
    if (ret_val != 0)
    {
        throw(runtime_error("WSA failed to start up!Error code: "
            + to_string(WSAGetLastError())));
    }
    m_SocketServer = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_SocketServer == INVALID_SOCKET)
    {      
        throw(runtime_error("Failed to create server socket!Error code: "
            + to_string(WSAGetLastError())));
    }
    //
    ret_val = ::bind(m_SocketServer, (SOCKADDR*)&addr_svr, addr_len);
    if (ret_val != 0)
    {
        throw(runtime_error("Failed to bind server socket!Error code: "
            + to_string(WSAGetLastError())));
    }
    //
    ret_val = ::listen(m_SocketServer, SOMAXCONN);
    if (ret_val == SOCKET_ERROR)
    {
        throw(runtime_error("Server socket failed to listen!Error code: "
            + to_string(WSAGetLastError())));
        ::WSACleanup();
    }
    //以上是看不懂也没看过的代码段，仅仅是把cerr输出改成了throw，更符合C++特点和用法
    
    //基本是从原始代码抄的，用于创建等待客户端连入的线程
    //实参WaitThread是真正用于与等待客户端连入的线程
    //实参（LPOVID）this，是WaitThread运行时收到的唯一参数。也就是说，线程运行起来，唯一参数就代表它是属于哪一个具体Server对象的。当然，每次Open打开端口才会创建一个等待线程，关闭后，这个线程就结束了。
    HANDLE h_thread = CreateThread(nullptr, 0, WaitThread, (LPVOID)this, 0, nullptr);
    if (h_thread == NULL)
    {
       WSACleanup();
       throw(runtime_error("Failed to create a new thread!Error code: " 
            + to_string(WSAGetLastError())));
    }
    ::CloseHandle(h_thread);                                 
}

/*************************************************************************
【函数名称】Server::Close
【函数功能】关闭端口监听，停止等待客户端连接
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Server::Close(){
    //不在等待（监听）状态下，直接返回，不会执行任何实质操作
    //请大家回顾下Server的析构，它就是调用了Close。如果没有打开端口，就没有连接过，也不需要任何清理。
    if (!m_bIsListening) {
        return;
    }
    //改变为非监听状态
    m_bIsListening = false;
    //逐一断开现有连接，销毁指针对应的Connection实体
    for (int i = m_Connections.size() - 1; i >= 0 ; i--) {
        m_Connections[i]->Disconnect();
        Connection* p = m_Connections[i];
        delete p;
    }
    //清空保存的客户端指针数组(vector)
    m_Connections.clear();
    //关闭服务端socket，抄原始代码+猜测
    closesocket(m_SocketServer);
    //始终不知道这句的确切含义，也不需要，抄原始代码
    WSACleanup();
}

/*************************************************************************
【函数名称】Server::IsConnected
【函数功能】返回指定客户端连接能否通信（是否处于连接状态）
【参数】入口参数，表示客户端连接索引号（从0开始）
【返回值】true表示可以通信，false表示不存在这一连接或已断开/无法通信
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Server::IsConnected(unsigned int idx) {
    if (idx >= m_Connections.size()) {
        return false;
    }
    return m_Connections[idx]->IsConnected();
}

/*************************************************************************
【函数名称】Server::DisConnected
【函数功能】断开指定客户端连接
【参数】入口参数，表示客户端连接索引号（从0开始）
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Server::DisConnect(unsigned int idx) {
    if (idx >= m_Connections.size()) {
        return;
    }
    m_Connections[idx]->Disconnect();
}

/*************************************************************************
【函数名称】Server::GetAvgToSendCount
【函数功能】获得全部活动连接的待发送消息数量均值
【参数】无）
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-05-10
【更改记录】
    2020-05-15 由范静涛增加注释
*************************************************************************/
//获得全部活动连接的待发送消息数量均值
unsigned int Server::GetAvgToSendCount() {
    //全部待发送消息总和
    unsigned int ToSendCount = 0;
    //活动连接(可通信的连接)总和
    unsigned int ActiveConnectionCount = 0;
    for (unsigned int i = 0; i < m_Connections.size(); ++i) {
        ToSendCount += m_Connections[i]->ToSendCount();
        ActiveConnectionCount += m_Connections[i]->IsConnected();
    }
    return ToSendCount / (float)ActiveConnectionCount;
}


/*************************************************************************
【函数名称】Server::WaitThread
【函数功能】等待连接线程函数
【参数】入口参数，表示线程所属Server的指针
【返回值】无意义，仅函数形式需要
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-4-24
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和参数接口修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
DWORD WINAPI Server::WaitThread(LPVOID lpParameter){
    //实际等待客户端连接的线程函数，基本来自于原始代码，稍作改动
    //Open被调用至Close被调用的中间时间段上，这个函数只运行一次，所以内部有循环，使用线程是考虑与程序的主线程（黑框也有主线程）分离。
    //唯一形参，值来自于CreateThread函数的第4参数
    //这是一个静态函数，是类的行为，所以此函数里没有this指针，那么此函数如何向所属Server添加客户端呢？就靠这个形参了

    //实参是Server*，代表此线程对应的“服务器”（Server对象），所以，用一个局部指针变量svr指向“服务器”。通过强制类型转化实现对Owner的赋值
    Server* svr = (Server*)lpParameter;
    //只有这个线程运行起来了，”服务器“才是真正开始等待（监听）客户端的连接请求
    svr->m_bIsListening = true;
    //如果”服务器“的IsListening为true，才循环等待客户端连接请求。如果”服务器“的Close被调用了，则IsListening会变为false，就不用继续等待了。
    while (svr->m_bIsListening)
    {
        //抄原始代码，原始代码的类私有成员前要加上svr->，同样是因为没有静态成员函数，没有this指针了。
        SOCKET sock_clt = ::accept(svr->m_SocketServer, (SOCKADDR*)&svr->addr_clt, &svr->addr_len);
        if (sock_clt == INVALID_SOCKET){
            break;
        }
        char* paddr = inet_ntoa(svr->addr_clt.sin_addr);
        strcpy(svr->buf_ip, paddr);
        //以下函数只适合VS，已替换为前面两行
        //::InetNtop(svr->addr_clt.sin_family, &svr->addr_clt, svr->buf_ip, IP_BUF_SIZE);
        //以上是抄原始代码
        
        //new一个Connection对象，指针放在temp里。构造函数的两个参数一个来自于原始代码，另一个表示”服务器“的消息队列。也就是说，所有Connection将都向”服务器“的消息队列里添加收到的文字信息。
        Connection* temp = new Connection(sock_clt, (unsigned int)(svr->m_Connections.size()), svr->m_Messages);
        //将temp（新建的客户端Connection对象的指针）添加到”服务器“的Connections数组（vector）中
        svr->m_Connections.push_back(temp);
    }
    //退出循环就结束线程，不继续监听了
    return 0;
}

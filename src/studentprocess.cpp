/*************************************************************************
【文件名】studentprocess.cpp
【功能模块和目的】学生业务流程类定义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-19 由范静涛修改为从QObject继承，用信号实现界面更新
    2020-05-19 由范静涛增加了日志输出QDebug
*************************************************************************/

#include "studentprocess.h"
#include <windows.h>
#include <QDebug>

/*************************************************************************
【函数名称】StudentProcess::StudentProcess
【函数功能】构造函数
【参数】入口参数，父对象指针，默认nullptr
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛删除界面空间指针参数，用信号实现界面更新
    2020-05-19 由范静涛修正了未初始化消息接收处理线程句柄为nullptr的错误逻辑
*************************************************************************/
StudentProcess::StudentProcess(QObject* Parent): QObject(Parent), IsInClass(m_bIsInClass)
{
    //准备一个用于远程登录的消息，包含自己的用户名和密码
    //GetLoginedUser获取的是本地登录的账户指针
    m_RemoteLoginMsg = User::GetLoginedUser()->ToMessage();
    //用私有成员保存界面控件指针
    //初始化内部状态
    m_bIsInClass = false;
    //新建socket客户端
    m_pClient = new Client();
    //新建音频播放对象
    m_pVoicePlayer = new Audio();
    //初始化消息接收处理线程句柄为空
    m_hMsgReceiveThread = nullptr;

    qDebug("SP: Created");
}

/*************************************************************************
【函数名称】StudentProcess::~StudentProcess
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
StudentProcess::~StudentProcess() {
    //退出课堂
    ExitClass();
    //释放socket客户端和音频对象
    delete m_pClient;
    m_pClient = nullptr;
    delete m_pVoicePlayer;
    m_pVoicePlayer = nullptr;
    qDebug("SP: Destroyed");
}

/*************************************************************************
【函数名称】StudentProcess::EnterClass
【函数功能】进入课堂
【参数】入口参数，教师端IP地址
【返回值】true表示当前连接状态（是否在课堂里）
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛修改未用信号实现界面更新
*************************************************************************/
bool StudentProcess::EnterClass(const QString& IP) {
    //如果已经进入课堂，则不执行任何操作
    if (m_bIsInClass) {
        return m_bIsInClass;
    }

    //客户端连接到服务器
    string IpStr = IP.toStdString();
    m_pClient->ConnectTo(IpStr, PORTNUM);

    //开始计时
    qDebug() << "SP: trying to connect to server @ " << QString::fromStdString(IpStr);

    DWORD BeginTime = GetTickCount();
    DWORD EndTime = BeginTime;
    do {
        //更新当前时间
        EndTime = GetTickCount();

        //6-18，休眠一小段时间，防止跑崩了，来自李浦豪同志的建议
        Sleep(SLEEPINWAITMS);

    //30秒内未连接，则继续循环,6-18改为10秒,6-19又改回来了
    }while(EndTime - BeginTime <= WAITFORCONNMS && !m_pClient->IsConnected);

    //连接失败，返回
    if (!m_pClient->IsConnected) {
        qDebug() << "SP: connect fail after " << EndTime - BeginTime << " ms";
        //发出“退出课堂信号”
        emit ClassExited();
        return false;
    }

    //更新为以进入课堂状态
    m_bIsInClass = true;
    //发出“进入课堂信号”
    qDebug() << "SP: connected after " << EndTime - BeginTime << " ms";
    emit ClassEntered();
    //创建消息接收和处理线程
    m_hMsgReceiveThread = CreateThread(nullptr, 0, MsgReceiveThread, (LPVOID)this, 0, nullptr);
    if (m_hMsgReceiveThread == nullptr) {
        qDebug() << "SP: failed to create a MessageRecv&Process thread!";
        throw(runtime_error("Failed to create a MessageRecv&Process thread!"));
    }
    //第一时间向教师端发送登录用的用户名密码消息
    qDebug() << "SP: send ID & Password";
    m_pClient->Send(m_RemoteLoginMsg);
    //开始播放声音(如果从教师端收到了)
    m_pVoicePlayer->StartPlay();

    return m_bIsInClass;
}

/*************************************************************************
【函数名称】StudentProcess::ExitClass
【函数功能】退出课堂
【参数】无
【返回值】true表示当前连接状态（是否在课堂里）
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛修改未用信号实现界面更新
*************************************************************************/
bool StudentProcess::ExitClass() {
    //如果未在课堂中，检查是否消息接收处理线程自主退出了
    if (!m_bIsInClass) {
        if(m_hMsgReceiveThread != nullptr) {
            qDebug() << "SP: clean MsgRecv&Process thread handle";
            CloseHandle(m_hMsgReceiveThread);
            m_hMsgReceiveThread = nullptr;
        }
        return m_bIsInClass;
    }

    //断开与服务器连接
    m_pClient->DisConnect();
    //停止播放音频
    m_pVoicePlayer->StopPlay();
    //清空待播放音频队列
    m_pVoicePlayer->Clear();
    //更新为未上课状态
    m_bIsInClass = false;
    //等待消息接收和处理线程结束
    qDebug() << "SP: wait MsgRecv&Process thread end";
    WaitForSingleObject(m_hMsgReceiveThread, WAITMS);
    //清理线程句柄
    qDebug() << "SP: clean MsgRecv&Process thread handle";
    CloseHandle(m_hMsgReceiveThread);
    m_hMsgReceiveThread = nullptr;
    //发出“退出课堂信号”
    emit ClassExited();
    //返回当前是否在课堂中
    return m_bIsInClass;
}

/*************************************************************************
【函数名称】StudentProcess::Send
【函数功能】向教师端发送文字消息
【参数】入口参数，QString字符串
【返回值】true表示当前连接状态（是否在课堂里）
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
bool StudentProcess::Send(const QString& Msg) {
    //不在课堂
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    //未连接教师端
    if (!m_pClient->IsConnected) {
        return m_pClient->IsConnected;
    }
    //装换格式并发送
    qDebug() << "SP: send Message: " << Msg;
    string MsgStr = Msg.toStdString();
    return m_pClient->Send(Message::FromString(MsgStr));
}

//6-17
//发送答案信息
/*************************************************************************
【函数名称】StudentProcess::SendAnsw
【函数功能】向教师端发送答案消息
【参数】入口参数，QString字符串
【返回值】true表示当前连接状态（是否在课堂里）
【开发者及日期】曹展翔 2020-6-17
【更改记录】2020-6-21添加注释
*************************************************************************/
bool StudentProcess::SendAnsw(const QString& Answ) {
    //不在课堂
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    //未连接教师端
    if (!m_pClient->IsConnected) {
        return m_pClient->IsConnected;
    }
    //装换格式并发送
    qDebug() << "SP: send Answer: " << Answ;
    string AnswStr = Answ.toStdString();
    return m_pClient->Send(Message::FromAnsw(AnswStr));
}

//6-17
//发送注意力信息
/*************************************************************************
【函数名称】StudentProcess::SendAnsw
【函数功能】向教师端发送注意力消息
【参数】入口参数，QString字符串
【返回值】true表示当前连接状态（是否在课堂里）
【开发者及日期】曹展翔 2020-6-17
【更改记录】2020-6-21添加注释
*************************************************************************/
bool StudentProcess::SendActProp(const QString& ActProp) {
    //不在课堂
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    //未连接教师端
    if (!m_pClient->IsConnected) {
        return m_pClient->IsConnected;
    }
    //装换格式并发送
    qDebug() << "SP: send Active Proportion: " << ActProp;
    string ActPropStr = ActProp.toStdString();
    return m_pClient->Send(Message::FromActProp(ActPropStr));
}

/*************************************************************************
【函数名称】StudentProcess::MsgRecieveThread
【函数功能】接收并处理消息的线程函数
【参数】入口参数，表示线程所属的StudentProcess对象指针
【返回值】无意义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛修改未用信号实现界面更新
    2020-05-19 由范静涛修正了jpg空间未释放的错误逻辑
*************************************************************************/
DWORD WINAPI StudentProcess::MsgReceiveThread(LPVOID lpParameter) {
    //因为实参是StudentProcess*，所以可以强制类型装换
    StudentProcess* pProcess = (StudentProcess*)lpParameter;
    //用于接收的临时消息存储对象
    Message TempMessage;
    //接收到的消息的类型
    MessageType Type;
    //在课堂且通信中，始终执行循环
    while (pProcess->IsInClass && pProcess->m_pClient->IsConnected) {
        //有待处理消息，则放入TempMessage里
        if (pProcess->m_pClient->GetAndRemoveFirstRecvMsg(TempMessage)) {
            //获得消息类型
            Type = TempMessage.Type();
            //如果是文本信息消息，转换成为本对象，发出信号要求界面更新
            if (Type == MSG_STRING) {
                qDebug() << "SP: recieved chat message: " << QString::fromStdString(Message::ToString(TempMessage));
                emit pProcess->RecvChat(QString::fromStdString(Message::ToString(TempMessage)));
            }
            else if(Type == MSG_QUES)       //6-16添加，如果是问题信息，转换成文本对象，发出信号
            {
                qDebug() << "SP: recieved question message: " << QString::fromStdString(Message::ToString(TempMessage));
                emit pProcess->RecvQues(QString::fromStdString(Message::ToString(TempMessage)));
            }
            else if (Type == MSG_CHOICE)    //6-17添加，如果是选择题信息，转换成文本对象，发出信号
            {
                string Ques;
                string OptionA;
                string OptionB;
                string OptionC;
                string OptionD;

                Message::ToChoice(TempMessage, Ques, OptionA, OptionB, OptionC, OptionD);

                emit pProcess->RecvChoice(QString::fromStdString(Ques), QString::fromStdString(OptionA), QString::fromStdString(OptionB), QString::fromStdString(OptionC), QString::fromStdString(OptionD));
            }

            //如果是音频帧消息，放入待播放音频队列中
            else if (Type == MSG_AUDIO) {
                qDebug() << "SP: recieved audio frame message " << TempMessage.Size << " bytes";
                pProcess->m_pVoicePlayer->AppendFrameToPlay(Message::ToAudioFrame(TempMessage));
            }
            //如果是Jpg图像消息，界面后刷到界面上
            else if (Type == MSG_JPG) {
                char* pJpgData = nullptr;
                unsigned int JpgSize = 0;
                Message::ToJpgData(TempMessage, &pJpgData, &JpgSize);
                QImage qImg;
                qImg.loadFromData((unsigned char*)pJpgData, JpgSize, "jpg");
                qDebug() << "SP: recieved jpg message " << JpgSize << " bytes";
                //2020-05-19 修正了逻辑，之前没有释放开辟的空间
                delete [] pJpgData;
                //发出收到图像信号，要求界面更新
                emit pProcess->RecvScreen(qImg);
            }
            //6-19，如果是即将下课的消息，发送发送注意力消息的信号
            else if (Type == MSG_SOONEXIT)
            {
                emit pProcess->RecvSoonExit();
            }
            //如果是退出消息，跳出消息接收处理循环，相当退出课堂
            else if (Type == MSG_EXIT) {
                pProcess->m_bIsInClass = false;
                qDebug() << "SP: disconnected socket";
                //发出退出课堂信号
                emit pProcess->ClassExited();
                break;
            }
        }
    }
    return 0;
}

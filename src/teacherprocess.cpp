/*************************************************************************
【文件名】teacherprocess.cpp
【功能模块和目的】教师业务流程类定义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-19 由范静涛修改为信号机制通知界面更新
*************************************************************************/
#include "teacherprocess.h"
#include <string>
#include "user.h"
#include "student.h"
#include <QDebug>
using namespace std;

/*************************************************************************
【函数名称】TeacherProcess::TeacherProcess
【函数功能】构造函数
【参数】入口参数，父对象指针，默认为nullptr
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛删除了界面类控件指针，修改为信号机制通知界面更新
*************************************************************************/
TeacherProcess::TeacherProcess(QObject* Parent)
    : QObject(Parent), IsInClass(m_bIsInClass),
      IsScreenSharing(m_bIsScreenSharing), IsVoiceSharing(m_bIsVoiceSharing) {
    //新建Server
    m_pServer = new Server();
    //新建录音对象
    m_pVoiceCapture = new Audio();
    //更新为未在上课状态
    m_bIsInClass = false;
    //更新为未在屏幕共享状态
    m_bIsScreenSharing = false;
    //更新为未在语音直播状态
    m_bIsVoiceSharing = false;
    //初始化所有线程句柄为空
    m_hScreeShareThread = nullptr;
    m_hVoiceShareThread = nullptr;
    m_hMsgReceiveThread = nullptr;
    //初始化三个数组，其实不必要，怕后面写忘了，放在这里作提示
    m_Names.clear();
    m_Passwords.clear();
    m_ConnectionIdx.clear();
    m_TimeInfoList.clear();  //6-18，清空
    qDebug() << "TP: Created";
}

/*************************************************************************
【函数名称】TeacherProcess::~TeacherProcess
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
TeacherProcess::~TeacherProcess(){
    //下课
    EndClass();
    //释放动态开辟的对象
    delete m_pServer;
    m_pServer = nullptr;
    delete m_pVoiceCapture;
    m_pVoiceCapture = nullptr;
}

/*************************************************************************
【函数名称】TeacherProcess::BeginClass
【函数功能】开始上课
【参数】无
【返回值】true为当前正在上课
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛修改为信号机制通知界面更新
*************************************************************************/
bool TeacherProcess::BeginClass(){
    //已是上课状态，什么都不做
    if (m_bIsInClass) {
        return m_bIsInClass;
    }
    //清空三个数组，相当于没有人登录了
    m_Names.clear();
    m_Passwords.clear();
    m_ConnectionIdx.clear();
    m_TimeInfoList.clear();  //6-18，清空
    //所有学生对象下线，之前登录状态都不算
    User::OfflineAllStudents();
    //打开服务端
    qDebug() << "TP: open server";
    m_pServer->Open(PORTNUM);
    //更新状态为上课中
    m_bIsInClass = true;
    //创建并启动消息接收与处理线程
    m_hMsgReceiveThread =  CreateThread(nullptr, 0, MsgReceiveThread, (LPVOID)this, 0, nullptr);
    if (m_hMsgReceiveThread == nullptr) {
        qDebug() << "TP: failed to create a MsgRecv&Process thread";
        throw(runtime_error("Failed to create a MsgRecv&Process thread!"));
    }
    //发送开始上课信号，要求界面更新
    qDebug() << "TP: class begin";
    emit ClassBegined();
    return m_bIsInClass;
}

/*************************************************************************
【函数名称】TeacherProcess::EndClass
【函数功能】下课
【参数】无
【返回值】true为当前正在上课（虽然是下课功能，但返回值为真时正在上课）
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛修改为信号机制通知界面更新
*************************************************************************/
bool TeacherProcess::EndClass() {
    //已下课，则检查消息接收与处理线程是否自主退出了
    if (!m_bIsInClass)
    {
        //6-19
        //SaveToFile(STUTIMEINFOFILE);

        if(m_hMsgReceiveThread != nullptr) {
            qDebug() << "TP: clean MsgRecv&Process thread handle";
            CloseHandle(m_hMsgReceiveThread);
            m_hMsgReceiveThread = nullptr;
        }
        return m_bIsInClass;
    }

    //6-19，获取时间
    time_t Time = time(NULL);
    char cTime[64] = {0};
    strftime(cTime, sizeof(cTime) - 1, "%Y-%m-%d %H:%M:%S", localtime(&Time));     //年-月-日 时-分-秒
    string sEndTime = cTime;

    //老师下课学生端还在线的同学直接记录结束时间为endclass的时候
    for (unsigned int i = 0; i < m_TimeInfoList.size(); i++)
    {
        if (m_TimeInfoList[i].GetEndTime() == "")
        {
            m_TimeInfoList[i].SetEndTime(sEndTime);
        }
    }

    //6-19，保存到文件
    SaveToFile(STUTIMEINFOFILE);

    //停止可能正在的屏幕共享
    StopScreenShare();
    //停止可能正在的语音直播
    StopVoiceShare();
    //关闭服务端
    qDebug() << "TP: server close";
    m_pServer->Close();
    //更新未在上课状态
    m_bIsInClass = false;
    //等待消息接收和处理线程结束
    qDebug() << "TP: wait for MsgRecv&Process thread exit";
    WaitForSingleObject(m_hMsgReceiveThread, WAITMS);
    qDebug() << "TP: clean MsgRecv&Process thread handle";
    CloseHandle(m_hMsgReceiveThread);
    m_hMsgReceiveThread = nullptr;

    //所有人下线，重置状态
    m_Names.clear();
    m_Passwords.clear();
    m_ConnectionIdx.clear();
    m_TimeInfoList.clear();
    qDebug() << "TP: offline all students";
    User::OfflineAllStudents();

    //发送下课信号，要求界面更新
    qDebug() << "TP: class end";
    emit ClassEnded();

    return m_bIsInClass;
}

/*************************************************************************
【函数名称】TeacherProcess::StartScreenShare
【函数功能】开始屏幕共享
【参数】无
【返回值】true为当前正在屏幕共享
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
bool TeacherProcess::StartScreenShare() {
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    if (m_bIsScreenSharing) {
        return m_bIsScreenSharing;
    }
    m_hScreeShareThread = CreateThread(nullptr, 0, ScreenShareThread, (LPVOID)this, 0, nullptr);
    if (m_hScreeShareThread == nullptr) {
        qDebug() << "TP: failed to create a new screen share thread";
        throw(runtime_error("Failed to create a new screen share thread!"));
    }
    m_bIsScreenSharing = true;
    qDebug() << "TP: start screen share";
    return m_bIsScreenSharing;
}

/*************************************************************************
【函数名称】TeacherProcess::StopScreenShare
【函数功能】停止屏幕共享
【参数】无
【返回值】true为当前正在屏幕共享
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
bool TeacherProcess::StopScreenShare() {
    if (!m_bIsScreenSharing) {
        return m_bIsScreenSharing;
    }
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    m_bIsScreenSharing = false;
    qDebug() << "TP: wait for screen share thread exit";
    WaitForSingleObject(m_hScreeShareThread, WAITMS);
    qDebug() << "TP: clean screen share thread handle";
    CloseHandle(m_hScreeShareThread);
    m_hScreeShareThread = nullptr;
    qDebug() << "TP: stop screen share";
    return m_bIsScreenSharing;
}

/*************************************************************************
【函数名称】TeacherProcess::StartVoiceShare
【函数功能】开始语音直播
【参数】无
【返回值】true为当前正在语音直播
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
bool TeacherProcess::StartVoiceShare() {
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    if (m_bIsVoiceSharing) {
        return m_bIsVoiceSharing;
    }
    m_hVoiceShareThread = CreateThread(nullptr, 0, VoiceShareThread, (LPVOID)this, 0, nullptr);
    if (m_hVoiceShareThread == nullptr) {
        qDebug() << "TP: failed to create a new voice share thread";
        throw(runtime_error("Failed to create a new voice share thread!"));
    }
    m_pVoiceCapture->StartRec();
    m_bIsVoiceSharing = true;
    qDebug() << "TP: start voice share";
    return m_bIsVoiceSharing;
}

/*************************************************************************
【函数名称】TeacherProcess::StopVoiceShare
【函数功能】停止语音直播
【参数】无
【返回值】true为当前正在语音直播
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
bool TeacherProcess::StopVoiceShare() {
    if (!m_bIsVoiceSharing) {
        return m_bIsVoiceSharing;
    }
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    m_pVoiceCapture->StopRec();
    m_bIsVoiceSharing = false;
    qDebug() << "TP: wait for voice share thread exit";
    WaitForSingleObject(m_hVoiceShareThread, WAITMS);

    CloseHandle(m_hVoiceShareThread);
    m_hVoiceShareThread = nullptr;
    qDebug() << "TP: clean voice share thread handel";
    AudioFrame TempFrame;
    while(m_pVoiceCapture->RecordFramesCount() > 0) {
        m_pVoiceCapture->GetAndRemoveFirstRecorded(TempFrame);
    }
    qDebug() << "TP: clean voice share queue";
    qDebug() << "TP: stop voice share";
    return m_bIsVoiceSharing;
}

/*************************************************************************
【函数名称】TeacherProcess::SendStrChat
【函数功能】群发消息
【参数】入口参数，消息内容QString
【返回值】是否发送成功
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
bool TeacherProcess::SendStrChat(const QString& ChatStr) {
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    std::string ToSend = ChatStr.toStdString();
    Message Chat(MSG_STRING, (unsigned char*)ToSend.c_str(), ToSend.length());
    m_pServer->SendToAll(Chat);
    qDebug() << "TP: send to all message: " << ChatStr;
    return m_bIsInClass;
}

//6-16
//照上面的葫芦画瓢                                                  //要改为单发
/*************************************************************************
【函数名称】TeacherProcess::SendQues
【函数功能】单发问题消息
【参数】入口参数，消息内容QString，要发送用户的名字的引用
【返回值】是否发送成功
【开发者及日期】曹展翔 2020-6-16
【更改记录】
*************************************************************************/
bool TeacherProcess::SendQues(const QString& Ques, QString& SendToName)
{
    unsigned int randidex = 0;

    srand((int)time(NULL));

    if (!m_bIsInClass) {
        return m_bIsInClass;
    }
    std::string ToSend = Ques.toStdString();
    //构造Message对象
    Message QuesMes(MSG_QUES, (unsigned char*)ToSend.c_str(), ToSend.length());

    //随机idex
    do
    {
        randidex = rand() % MAXUSERNUM;

    } while (!(m_pServer->SendTo(randidex, QuesMes)));  //直到发送成功

    SendToName = QString::fromStdString(m_Names[randidex]); //名字

    qDebug() << "TP: send to all message: " << Ques;
    return m_bIsInClass;
}

//6-16
//照上面的葫芦画瓢
/*************************************************************************
【函数名称】TeacherProcess::SendChoice
【函数功能】群发选择题消息
【参数】入口参数，消息内容均为QString
【返回值】是否发送成功
【开发者及日期】曹展翔 2020-6-16
【更改记录】
*************************************************************************/
bool TeacherProcess::SendChoice(const QString& Ques, const QString& OptionA, const QString& OptionB, const QString& OptionC, const QString& OptionD) {
    if (!m_bIsInClass) {
        return m_bIsInClass;
    }

    //转换为Message对象
    Message QuesMes = Message::FromChoice(Ques.toStdString(), OptionA.toStdString(), OptionB.toStdString(), OptionC.toStdString(), OptionD.toStdString());
    m_pServer->SendToAll(QuesMes);
    qDebug() << "TP: send to all message: " << Ques << " A:" << OptionA << " B:" << OptionB << " C:" << OptionC << " D:" << OptionD;
    return m_bIsInClass;
}

//6-19
//继续画
/*************************************************************************
【函数名称】TeacherProcess::SendSoonExit
【函数功能】群发即将下课消息
【参数】无
【返回值】是否发送成功
【开发者及日期】曹展翔 2020-6-16
【更改记录】
*************************************************************************/
bool TeacherProcess::SendSoonExit()
{
    if (!m_bIsInClass)
    {
        return m_bIsInClass;
    }

    //构造Message对象
    Message SoonExitMsg = Message::MakeSoonExitMsg();
    m_pServer->SendToAll(SoonExitMsg);
    qDebug() << "TP: send to all message: Soon Exit Class";
    return m_bIsInClass;
}


/*************************************************************************
【函数名称】TeacherProcess::ScreenShareThread
【函数功能】屏幕共享线程函数
【参数】入口参数，TeacherProcess的指针
【返回值】无意义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
*************************************************************************/
DWORD WINAPI TeacherProcess::ScreenShareThread(LPVOID lpParameter){
    TeacherProcess* pProcess = (TeacherProcess*)lpParameter;
    Image TempImage;
    while (pProcess->IsScreenSharing) {
        //如果平均未发消息不足10，就抓屏幕，转换成jpg消息，从server群发出去
        if (pProcess->m_pServer->GetAvgToSendCount() <= 10) {
            CapScreenTo(TempImage);
            pProcess->m_pServer->SendToAll(Message::FromImageJpg(TempImage));
            qDebug() << "TP: send to all screen";
        }
        //限定帧率：两次抓屏间隔不小于40ms（1/24秒）
        Sleep(40);
    }
    return 0;
}

/*************************************************************************
【函数名称】TeacherProcess::VoiceShareThread
【函数功能】语音线程函数
【参数】入口参数，TeacherProcess的指针
【返回值】无意义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 范静涛优化了代码效率，每次都取出一个音频帧，不管是否发送，都
*************************************************************************/
DWORD WINAPI TeacherProcess::VoiceShareThread(LPVOID lpParameter){
    TeacherProcess* pProcess = (TeacherProcess*)lpParameter;
    AudioFrame TempFrame;
    while (pProcess->IsVoiceSharing) {
        //取出一个录制好的音频帧
        if (pProcess->m_pVoiceCapture->GetAndRemoveFirstRecorded(TempFrame)) {
            qDebug() << "TP: remove an audio frame";
            //如果平均未发消息不足10，就抓屏幕，转换成音频消息，从server群发出去
            if (pProcess->m_pServer->GetAvgToSendCount() <= 10) {
                pProcess->m_pServer->SendToAll(Message::FromAudio(TempFrame));
                qDebug() << "TP: send to all an audio frame";
            }
        }
    }
    return 0;
}

/*************************************************************************
【函数名称】TeacherProcess::MsgRecieveThread
【函数功能】消息接收和处理线程函数
【参数】入口参数，TeacherProcess的指针
【返回值】无意义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛修改为信号机制通知界面更新
*************************************************************************/
DWORD WINAPI TeacherProcess::MsgReceiveThread(LPVOID lpParameter) {
    TeacherProcess* pProcess = (TeacherProcess*)lpParameter;
    //临时消息pair，两个分量分别从哪个连接收到的消息、收到了什么消息
    pair<unsigned int, Message> TempPair;
    MessageType Type;
    while (pProcess->IsInClass) {
        //如果有待处理消息，则取出最早的一个
        if (pProcess->m_pServer->GetAndRemoveFirstMessage(TempPair)) {
            //获取消息类型
            Type = TempPair.second.Type();
            //如果是文字信息，就通知界面更新
            if (Type == MSG_STRING) {
                qDebug() << "TP: recieved s string from connection " << TempPair.first;
                emit pProcess->RecvChat(QString::fromStdString(pProcess->m_Names[TempPair.first]), QString::fromStdString(Message::ToString(TempPair.second)));

                //6-21,使学生能看到其他同学发送的消息
                std::string ToSend = (pProcess->m_Names[TempPair.first]) + ": " + Message::ToString(TempPair.second);
                Message Chat(MSG_STRING, (unsigned char*)ToSend.c_str(), ToSend.length());
                pProcess->m_pServer->SendToAll(Chat);
            }
            else if (Type == MSG_ANSW)  //6-17,答案消息，发送信号
            {
                qDebug() << "TP: recieved s answer from connection " << TempPair.first;
                emit pProcess->RecvAnsw(QString::fromStdString(pProcess->m_Names[TempPair.first]), QString::fromStdString(Message::ToString(TempPair.second)));
            }
            else if (Type == MSG_ACTPROP)   //注意力消息
            {
                qDebug() << "TP: recieved s active proportion from connection " << TempPair.first;

                //私有成员储存信息,再在下课时显示出来,6-18
                for (unsigned int i = 0; i < pProcess->m_TimeInfoList.size(); i++)
                {
                    if (pProcess->m_TimeInfoList[i].GetConnIdex() == TempPair.first)
                    {
                        pProcess->m_TimeInfoList[i].SetActInfo(Message::ToString(TempPair.second));
                    }
                }

            }
            //如果是用户名密码的登录消息
            else if (Type == MSG_ID_PSW ) {
                //这个学生已经登录了并正常通信中，则不允许重复登录；如果未登录过，但用户名密码不对，也是一样
                if (!pProcess->OnlineStudent(TempPair)) {
                    //断开连接
                    qDebug() << "TP: disconnect socket: " << TempPair.first;
                    pProcess->m_pServer->DisConnect(TempPair.first);
                }
                else
                {
                    //6-18，获取时间
                    time_t Time = time(NULL);
                    char cTime[64] = {0};
                    strftime(cTime, sizeof(cTime) - 1, "%Y-%m-%d %H:%M:%S", localtime(&Time));     //年-月-日 时-分-秒
                    string sBeginTime = cTime;

                    //6-18,上课时间
                    pProcess->m_TimeInfoList.push_back(StuTimeInfo(TempPair.first, pProcess->m_Names[TempPair.first], sBeginTime));
                }
            }
            //如果是退出消息，修改这个同学的登录状态
            else if (Type == MSG_EXIT)
            {
                //6-18，获取时间
                time_t Time = time(NULL);
                char cTime[64] = {0};
                strftime(cTime, sizeof(cTime) - 1, "%Y-%m-%d %H:%M:%S", localtime(&Time));     //年-月-日 时-分-秒
                string sEndTime = cTime;

                //6-18，设置结束时间
                for (unsigned int i = 0; i < pProcess->m_TimeInfoList.size(); i++)
                {
                    if (pProcess->m_TimeInfoList[i].GetConnIdex() == TempPair.first)
                    {
                        pProcess->m_TimeInfoList[i].SetEndTime(sEndTime);
                    }
                }

                pProcess->OffLineStudent(TempPair.first);
            }
        }
    }
    return 0;
}

//6-18
//将返回值改为引用？
/*************************************************************************
【函数名称】TeacherProcess::GetTimeInfo
【函数功能】返回学生时间信息列表
【参数】需要修改的列表的引用
【返回值】是否发送成功
【开发者及日期】曹展翔 2020-6-18
【更改记录】
*************************************************************************/
void TeacherProcess::GetTimeInfo(vector<StuTimeInfo>& TimeInfoList)
{
    TimeInfoList = m_TimeInfoList;
}


//以下两个函数，不需要掌握
bool TeacherProcess::OnlineStudent(const pair<unsigned int, Message>& Pair){
    string Name;
    string Password;
    //获得要登录的用户在哪个Connection
    unsigned int ConnectionIdx = Pair.first;
    //获得用户名密码
    Message::ToStudentNameAndPassword(Pair.second, Name, Password);
    //在用户列表中按用户名密码搜索用户
    qDebug() << "TP: " << QString::fromStdString(Name) << "is trying to login";
    const User* pCurrent = User::GetUser(Name, Password);
    //未找到此用户，返回false
    if (pCurrent == nullptr) {
        qDebug() << "TP: no user " << QString::fromStdString(Name);
        return false;
    }
    //找到用户则转换为Student指针
    Student* pStudent = const_cast<Student*>(dynamic_cast<const Student*>(pCurrent));
    int CurrentConnectionIdx = 0;
    //判定指定学生用户当前是否在线，如在线，在哪个connection
    if (pStudent->IsOnline(CurrentConnectionIdx)) {
        //学生在线，在CurrentConnectionIdx
        //判定CurrentConnectionIdx对应的连接是否处于通信状态
        if (m_pServer->IsConnected(CurrentConnectionIdx)) {
            qDebug() << "TP: " << QString::fromStdString(Name) << "has logined at " << CurrentConnectionIdx;
            //在通信状态，未断开，返回false，不允许重复登录
            return false;
        }
        //学生在线, 但connection已经断开了，更新
        else {
            //设定在线
            qDebug() << "TP: " << QString::fromStdString(Name) << " is reset to " << CurrentConnectionIdx;
            pStudent->Online(ConnectionIdx);
            //更新之前记录的ConnectionIdx
            for (unsigned int i = 0; i < m_Names.size(); i++) {
                if (m_Names[i] == Name && m_Passwords[i] == Password) {
                    m_ConnectionIdx[i] = ConnectionIdx;
                    break;
                }
            }
            //发出信号，让界面更新学生列表
            emit RecvStudentLogin(QString::fromStdString(Name));
            return true;
        }
    }
    //学生不在线
    else {
        //设定在吸纳
        qDebug() << "TP: " << QString::fromStdString(Name) << " login is ok. at " << CurrentConnectionIdx;
        pStudent->Online(ConnectionIdx);
        //记录用户名密码和ConnectionIdx到数组
        m_Names.push_back(Name);
        m_Passwords.push_back(Password);
        m_ConnectionIdx.push_back(ConnectionIdx);


        //发出信号，让界面更新学生列表
        emit RecvStudentLogin(QString::fromStdString(Name));
        return true;
    }
}

void TeacherProcess::OffLineStudent(unsigned int Idx) {
    //搜索idx号connection对应的学生对象
    for (unsigned int i = 0; i < m_Names.size(); i++) {
        //搜索idx对相应的数组记录，获取用户名密码
        if (m_ConnectionIdx[i] == Idx) {
            //按用户名密码找学生对象指针
            const User* pCurrent = User::GetUser(m_Names[i], m_Passwords[i]);
            if (pCurrent == nullptr) {
                return;
            }
            Student* pStudent = const_cast<Student*>(dynamic_cast<const Student*>(pCurrent));
            //下线/登出学生对象
            qDebug() << "TP: " << QString::fromStdString(m_Names[i]) << " logout";
            pStudent->Offline();
            //删连接记录
            m_ConnectionIdx.erase(m_ConnectionIdx.begin() + i);
            //发出信号，还要求更新界面显示
            emit RecvStudentLogout(QString::fromStdString(m_Names[i]));
            //删用户名记录
            m_Names.erase(m_Names.begin() + i);
            //删密码记录
            m_Passwords.erase(m_Passwords.begin() + i);


            break;
        }
    }
}

//6-19
//将文件内容清空，重新写
void TeacherProcess::SaveToFile(const string& FileName)
{
    ofstream OutFile(FileName.c_str());

    if (OutFile.fail())
    {
        ofstream AOutFile(FileName.c_str(), ios::out | ios::app);
        AOutFile.close();
    }
    OutFile.close();
    OutFile.open(FileName.c_str());

    for (unsigned int i  = 0; i < m_TimeInfoList.size(); i++)
    {
        OutFile << m_TimeInfoList[i].GetName() << "    " << m_TimeInfoList[i].GetBeginTime() << "    " << m_TimeInfoList[i].GetEndTime() << "    " << m_TimeInfoList[i].GetActInfo() << "%" << endl;
    }

    OutFile.close();
}



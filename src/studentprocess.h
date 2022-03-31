/*************************************************************************
【文件名】studentprocess.h
【功能模块和目的】学生业务流程类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-19 由范静涛修改为从QObject继承，用信号实现界面更新
*************************************************************************/
#ifndef STUDENTPROCESS_H
#define STUDENTPROCESS_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QComboBox>
#include <QScrollArea>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <string>
#include "client.h"
#include "Message.h"
#include "audio.h"
#include "user.h"
#include "Constant.h"
using namespace std;

/*************************************************************************
【类名】StudentProcess
【功能】学生上课业务流程类
【接口说明】
    构造函数StudentProcess
    析构函数~StudentProcess
    函数EnterClass进入指定IP的课堂
    函数ExitClass退出课堂
    函数Send向教师发送文本消息
    常引用数据成员IsInClass表示是否在上课
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-12
【更改记录】
    2020-05-19 由范静涛修改为从QObject继承，用信号实现界面更新
*************************************************************************/
class StudentProcess : public QObject{
    Q_OBJECT
public:
    //构造函数
    explicit StudentProcess(QObject* Parent = nullptr);

    //析构函数
    ~StudentProcess();

    //禁用拷贝构造函数
    StudentProcess(const StudentProcess&) = delete;

    //禁用赋值运算符
    StudentProcess& operator=(const StudentProcess&) = delete;

    //进入课堂
    bool EnterClass(const QString& IP);

    //退出课堂
    bool ExitClass();

    //发送文本消息给教师
    bool Send(const QString& Msg);

    bool SendAnsw(const QString& Answ); //6-17

    bool SendActProp(const QString& ActProp); //6-18

    //是否在上课
    const bool& IsInClass;

signals:
    //进入课堂信号
    void ClassEntered();
    //退出课堂信号
    void ClassExited();

    //6-19
    void RecvSoonExit();

    //收到文字消息信号
    void RecvChat(QString Msg);

    //6-16
    void RecvQues(QString Ques);

    //6-17
    void RecvChoice(QString Ques, QString OptionA, QString OptionB, QString OptionC, QString OptionD);

    //收到屏幕共享消息信号
    void RecvScreen(QImage Img);

private:
    //是否在上课
    bool m_bIsInClass;

    //Socket客户端对象指针
    Client* m_pClient;

    //音频播放对象指针
    Audio* m_pVoicePlayer;

    //接收消息并处理消息的线程函数
    static DWORD WINAPI MsgReceiveThread(LPVOID lpParameter);
    //对收到的消息进行执行的线程句柄
    HANDLE m_hMsgReceiveThread;
    Message m_RemoteLoginMsg;
};

#endif // STUDENTPROCESS_H

#include "studentwindow.h"
#include "ui_studentwindow.h"
#include <QImage>
#include <QPixmap>
#include <QMessageBox>

StudentWindow::StudentWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StudentWindow)
{
    ui->setupUi(this);
    m_pProc = new StudentProcess(this);

    m_pAnswerWindow = nullptr;
    this->setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    //建立信号和槽的连接关系
    //一定要在m_pProc构造后才能建立
    //connect是建立联系，而非调用槽函数
    connect(m_pProc, SIGNAL(ClassEntered()), this, SLOT(ClassEntered()));
    connect(m_pProc, SIGNAL(ClassExited()), this, SLOT(ClassExited()));
    connect(m_pProc, SIGNAL(RecvSoonExit()), this, SLOT(RecvSoonExit())); //6-19
    connect(m_pProc, SIGNAL(RecvChat(QString)), this, SLOT(RecvChat(QString)));
    connect(m_pProc, SIGNAL(RecvQues(QString)), this, SLOT(RecvQues(QString)));     //6-16
    connect(m_pProc, SIGNAL(RecvChoice(QString, QString, QString, QString, QString)), this, SLOT(RecvChoice(QString, QString, QString, QString, QString)));     //6-17
    connect(m_pProc, SIGNAL(RecvScreen(QImage)), this, SLOT(RecvScreen(QImage)));
    m_pTimer = new QTimer();
    m_pTimer->setInterval(200);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    m_pTimer->start();      //6-18，200ms发一次，start里不用加200？
    //只显示关闭窗口按钮，没有？按钮
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    //设定控件是否可用
    UpdateControlEnabled(false);
}

StudentWindow::~StudentWindow()
{
    m_pTimer->stop();
    disconnect(m_pTimer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    delete m_pTimer;
    m_pTimer = nullptr;
    //先disconnected，再释放业务流程类
    disconnect(m_pProc, SIGNAL(ClassEntered()), this, SLOT(ClassEntered()));
    disconnect(m_pProc, SIGNAL(ClassExited()), this, SLOT(ClassExited()));
    disconnect(m_pProc, SIGNAL(RecvSoonExit()), this, SLOT(RecvSoonExit())); //6-19
    disconnect(m_pProc, SIGNAL(RecvChat(QString)), this, SLOT(RecvChat(QString)));
    disconnect(m_pProc, SIGNAL(RecvQues(QString)), this, SLOT(RecvQues(QString)));     //6-16
    disconnect(m_pProc, SIGNAL(RecvChoice(QString, QString, QString, QString, QString)), this, SLOT(RecvChoice(QString, QString, QString, QString, QString)));     //6-17
    disconnect(m_pProc, SIGNAL(RecvScreen(QImage)), this, SLOT(RecvScreen(QImage)));
    delete m_pProc;
    m_pProc = nullptr;
    delete ui;
}

void StudentWindow::UpdateControlEnabled(bool IsInClass) {
    //非上课状态，清空共享屏幕显示、清空聊天记录、清空聊天输入文本框
    if (!IsInClass) {
        ui->lblSharedScreen->clear();
        ui->cmbChatList->clear();
        ui->edtChat->clear();
    }
    //IP输入文本框在非上课状态可用
    ui->edtIP->setEnabled(!IsInClass);
    //进入课堂按钮在非上课状态可用
    ui->btnEnterClass->setEnabled(!IsInClass);
    //退出课堂按钮在上课状态可用
    ui->btnExitClass->setEnabled(IsInClass);
    //聊天输入框在上课状态可用
    ui->edtChat->setEnabled(IsInClass);
    //发送按钮在上课状态可用
    ui->btnSend->setEnabled(IsInClass);
}

void StudentWindow::on_btnEnterClass_clicked()
{
    QMessageBox msgBox;
    m_uActiveTime = 0;  //6-18,重新计时
    m_uInActiveTime = 0;    //6-18，重新计时

    //IP不为空在能进入课堂
    if (!ui->edtIP->text().isEmpty()) {
        //更新界面控件状态为非上课状态
        UpdateControlEnabled(false);
        //临时锁死IP输入框
        ui->edtIP->setEnabled(false);
        //临时锁死退出课堂按钮
        ui->btnExitClass->setEnabled(false);
        //尝试进入课堂
        if (!m_pProc->EnterClass(ui->edtIP->text()))
        {
            msgBox.setText("连接失败！");    //6-19，添加提示
            msgBox.exec();
        }
    }
}

void StudentWindow::on_btnExitClass_clicked()
{
    //6-18增加了注意力信息的发送
    double ActiveProp = 100.0 * m_uActiveTime / (1.0 * (m_uActiveTime + m_uInActiveTime));

    QString qsActiveProp = QString::number(ActiveProp, 'f', 2);

    m_pProc->SendActProp(qsActiveProp);

    Sleep(SLEEPMS);    //6-18,为了让消息发出去，或许可以休眠更短？

    m_pProc->ExitClass();
}

void StudentWindow::on_btnSend_clicked(){
    //聊天输入框有文字才能发送
    if (!ui->edtChat->text().isEmpty()) {
        //发送结束则清空聊天输入框
        if (m_pProc->Send(ui->edtChat->text())) {
            ui->edtChat->clear();
        }
    }
}

void StudentWindow::closeEvent (QCloseEvent* e) {
    //上课中禁止关闭窗口
    if (m_pProc->IsInClass) {
        e->ignore();
    }
}

void StudentWindow::TimerEvent() {
    //定时更新窗口标题
    if (m_pProc->IsInClass) {
        //上课中，窗口是激活状态
        if(this->isActiveWindow()) {
            m_uActiveTime++;
            this->setWindowTitle("学生 " + QString::fromStdString(User::GetLoginedUser()->GetName()) +  " 在线");
        }
        //上课中，窗口不是激活状态
        else {
            m_uInActiveTime++;
            this->setWindowTitle("学生 " + QString::fromStdString(User::GetLoginedUser()->GetName()) +  " 离开");
        }
    }
    else {
        //非上课
        this->setWindowTitle("学生 " + QString::fromStdString(User::GetLoginedUser()->GetName()) +  " 未在课堂中");
    }
}

//进入课堂信号
void StudentWindow::ClassEntered(){
    UpdateControlEnabled(true);
}

//退出课堂信号
void StudentWindow::ClassExited(){
    UpdateControlEnabled(false);
}

//6-19
//收到即将退出课堂的消息
void StudentWindow::RecvSoonExit()
{
    //6-18增加了注意力信息的发送
    double ActiveProp = 100.0 * m_uActiveTime / (1.0 * (m_uActiveTime + m_uInActiveTime));

    QString qsActiveProp = QString::number(ActiveProp, 'f', 2);

    m_pProc->SendActProp(qsActiveProp);
}

//收到文字消息信号
void StudentWindow::RecvChat(QString Msg){
    ui->cmbChatList->insertItem(0, Msg);
    ui->cmbChatList->setCurrentIndex(0);
}

//6-16
//收到题目消息
void StudentWindow::RecvQues(QString Ques)
{
    m_pAnswerWindow = new AnswerWindow(this, m_pProc, false, Ques);

    m_pAnswerWindow->exec();
    delete m_pAnswerWindow;
    m_pAnswerWindow = nullptr;

}

//6-17
//收到选择题消息
void StudentWindow::RecvChoice(QString Ques, QString OptionA, QString OptionB, QString OptionC, QString OptionD)
{
    m_pAnswerWindow = new AnswerWindow(this, m_pProc, true, Ques, OptionA, OptionB, OptionC, OptionD);

    m_pAnswerWindow->exec();
    delete m_pAnswerWindow;
    m_pAnswerWindow = nullptr;

}

//收到屏幕共享消息信号
void StudentWindow::RecvScreen(QImage Img){
    //收到的图像尺寸和滚动区尺寸不一致，则调整滚动区尺寸
    if (ui->scrollAreaWidgetContents->minimumSize() != Img.size()) {
        ui->scrollAreaWidgetContents->setMinimumSize(Img.size());
    }
    //收到的图像尺寸和Label尺寸不一致，则调整Label尺寸
    if (ui->lblSharedScreen->minimumSize() != Img.size()) {
        ui->lblSharedScreen->setMinimumSize(Img.size());
    }
    //刷新Label显示
    ui->lblSharedScreen->setPixmap(QPixmap::fromImage(Img));
}


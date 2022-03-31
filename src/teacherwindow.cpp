 #include "teacherwindow.h"
#include "ui_teacherwindow.h"
#include <QGuiApplication>
#include <QScreen>
#include <QSize>

TeacherWindow::TeacherWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TeacherWindow){
    ui->setupUi(this);
    m_pProc = new TeacherProcess(this);

    UpdateControlEnabled(false);    //6-18，设定按钮的上课状态为否

    m_pQuestionWindow = nullptr;    //6-16

    //设置一些按钮不可用---------------------------------------------------------------
    //建立信号和槽的连接关系
    //一定要在m_pProc构造后才能建立
    //connect是建立联系，而非调用槽函数
    connect(m_pProc, SIGNAL(ClassBegined()), this, SLOT(ClassBegined()));
    connect(m_pProc, SIGNAL(ClassEnded()), this, SLOT(ClassEnded()));
    connect(m_pProc, SIGNAL(RecvStudentLogin(QString)), this, SLOT(RecvStudentLogin(QString)));
    connect(m_pProc, SIGNAL(RecvStudentLogout(QString)), this, SLOT(RecvStudentLogout(QString)));
    connect(m_pProc, SIGNAL(RecvChat(QString, QString)), this, SLOT(RecvChat(QString, QString)));
    connect(m_pProc, SIGNAL(RecvAnsw(QString, QString)), this, SLOT(RecvAnswer(QString, QString)));   //6-17

    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
}

TeacherWindow::~TeacherWindow(){
    //先disconnected，再释放业务流程类
    disconnect(m_pProc, SIGNAL(ClassBegined()), this, SLOT(ClassBegined()));
    disconnect(m_pProc, SIGNAL(ClassEnded()), this, SLOT(ClassEnded()));
    disconnect(m_pProc, SIGNAL(RecvStudentLogin(QString)), this, SLOT(RecvStudentLogin(QString)));
    disconnect(m_pProc, SIGNAL(RecvStudentLogout(QString)), this, SLOT(RecvStudentLogout(QString)));
    disconnect(m_pProc, SIGNAL(RecvChat(QString, QString)), this, SLOT(RecvChat(QString, QString)));
    disconnect(m_pProc, SIGNAL(RecvAnsw(QString, QString)), this, SLOT(RecvAnswer(QString, QString)));   //6-17

    delete m_pProc;
    delete ui;
}

void TeacherWindow::UpdateControlEnabled(bool IsInClass){
    if (!IsInClass) {
        ui->cmbStudent->clear();
        ui->cmbChatList->clear();
        ui->edtChat->clear();
        ui->ckbScreenShare->setChecked(false);
        ui->ckbVoiceShare->setChecked(false);
    }
    ui->btnBeginClass->setEnabled(!IsInClass);
    ui->btnEndClass->setEnabled(IsInClass);
    ui->btnSend->setEnabled(IsInClass);
    ui->ckbVoiceShare->setEnabled(IsInClass);
    ui->ckbScreenShare->setEnabled(IsInClass);
    ui->edtChat->setEnabled(IsInClass);
    ui->btnSendQues->setEnabled(IsInClass); //6-17
    ui->btnRandQues->setEnabled(IsInClass); //6-17
}

void TeacherWindow::showEvent(QShowEvent* e) {
    int iWidth = QGuiApplication::screens()[0]->size().width();
    this->move((iWidth - this->width()) / 2, 0);
    e->accept();
}

void TeacherWindow::closeEvent (QCloseEvent* e) {
    if (m_pProc->IsInClass) {
        e->ignore();
    }
}

void TeacherWindow::on_btnBeginClass_clicked() {
    if (m_pProc->BeginClass()) {
        ui->cmbStudent->clear();
        ui->cmbChatList->clear();
    }
}

void TeacherWindow::on_ckbScreenShare_clicked()
{
    if (ui->ckbScreenShare->isChecked()) {
        ui->ckbScreenShare->setChecked(m_pProc->StartScreenShare());
    }
    else{
        ui->ckbScreenShare->setChecked(m_pProc->StopScreenShare());
    }
}

void TeacherWindow::on_ckbVoiceShare_clicked()
{
    if (ui->ckbVoiceShare->isChecked()) {
        ui->ckbVoiceShare->setChecked(m_pProc->StartVoiceShare());
    }
    else{
        ui->ckbVoiceShare->setChecked(m_pProc->StopVoiceShare());
    }
}

void TeacherWindow::on_btnEndClass_clicked()
{
    //6-19，发送即将下课的信息
    m_pProc->SendSoonExit();
    Sleep(SLEEPMS);    //休眠1秒，等待学生的注意力消息

    //6-18，传递事间列表的信息
    vector<StuTimeInfo> TimeInfoList;
    m_pProc->GetTimeInfo(TimeInfoList);

    ActiveInfoWindow* pActInfoWin = new ActiveInfoWindow(this, TimeInfoList);
    pActInfoWin->exec();
    delete pActInfoWin;
    pActInfoWin = nullptr;

    m_pProc->EndClass();
}

void TeacherWindow::on_btnSend_clicked(){
    if (m_pProc->SendStrChat(ui->edtChat->text())) {
        //ui->edtChat->clear();
    }
}

void TeacherWindow::ClassBegined(){
    UpdateControlEnabled(true);
}

void TeacherWindow::ClassEnded() {
    UpdateControlEnabled(false);
}

void TeacherWindow::RecvStudentLogin(QString Name){
    int idx = ui->cmbStudent->findText(Name);
    if (idx == -1) {
        ui->cmbStudent->insertItem(0, Name);
        idx  = 0;
    }
    ui->cmbStudent->setCurrentIndex(idx);
}

void TeacherWindow::RecvStudentLogout(QString Name) {
    int idx = ui->cmbStudent->findText(Name);
    if (idx != -1) {
        ui->cmbStudent->removeItem(idx);
    }
}

void TeacherWindow::RecvChat(QString Name, QString Msg){
        ui->cmbChatList->insertItem(0, (Name + ": " + Msg));
        ui->cmbChatList->setCurrentIndex(0);
}

//6-16
//收到答案
void TeacherWindow::RecvAnswer(QString Name, QString Answ)
{
    if (m_pQuestionWindow != nullptr)
    {
        m_pQuestionWindow->AddAnswer(Name, Answ);
    }
}

//6-16
//发送问题点击
void TeacherWindow::on_btnSendQues_clicked()
{
    m_pQuestionWindow = new QuestionWindow(this, m_pProc, true);
    m_pQuestionWindow->exec();
    delete m_pQuestionWindow;
    m_pQuestionWindow = nullptr;
}

//6-17
//随机提问点击
void TeacherWindow::on_btnRandQues_clicked()
{
    m_pQuestionWindow = new QuestionWindow(this, m_pProc, false);
    m_pQuestionWindow->exec();
    delete m_pQuestionWindow;
    m_pQuestionWindow = nullptr;
}

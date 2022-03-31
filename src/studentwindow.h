#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>
#include "studentprocess.h"
#include "answerwindow.h"

namespace Ui {
class StudentWindow;
}

class StudentWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StudentWindow(QWidget *parent = nullptr);
    ~StudentWindow();
    void closeEvent (QCloseEvent* e) override;
    void UpdateControlEnabled(bool IsInClass);
private slots:
    void on_btnEnterClass_clicked();
    void on_btnExitClass_clicked();
    void on_btnSend_clicked();
    void TimerEvent();
    //进入课堂信号
    void ClassEntered();
    //退出课堂信号
    void ClassExited();

    //6-19，即将退出课堂信号
    void RecvSoonExit();

    //收到文字消息信号
    void RecvChat(QString Msg);

    //6-16，收到题目信号（单发）
    void RecvQues(QString Ques);

    //6-17，收到选择题信号
    void RecvChoice(QString Ques, QString OptionA, QString OptionB, QString OptionC, QString OptionD);

    //收到屏幕共享消息信号
    void RecvScreen(QImage Img);
private:
    Ui::StudentWindow *ui;
    StudentProcess* m_pProc;        //业务流程类对象指针
    AnswerWindow* m_pAnswerWindow;  //6-16，答题界面类指针

    unsigned int m_uActiveTime; //6-18,200ms为一次
    unsigned int m_uInActiveTime; //6-18,200ms为一次

    QTimer* m_pTimer;
};

#endif // STUDENTWINDOW_H

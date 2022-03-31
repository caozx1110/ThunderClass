#ifndef ANSWERWINDOW_H
#define ANSWERWINDOW_H

#include "studentprocess.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDialog>
#include <ctime>

namespace Ui {
class AnswerWindow;
}

class AnswerWindow : public QDialog
{
    Q_OBJECT

public:
    //第二个参数：业务流程类对象指针
    //第三个参数：是否为发送至全班状态
    //之后的参数：题干+四个选项
    explicit AnswerWindow(QWidget *parent = nullptr, StudentProcess* pProc = nullptr, bool IsRecvAll = true, QString Ques = "", QString OptionA = "", QString OptionB = "", QString OptionC = "", QString OptionD = "");
    ~AnswerWindow();
    void closeEvent (QCloseEvent* e) override;

private slots:
    void on_btnSubmit_clicked();

private:
    Ui::AnswerWindow *ui;
    //业务流程类对象指针
    StudentProcess* m_pProc;

    //6-18，增加记录窗口开始和结束时间
    time_t m_tBeginTime;
    time_t m_tEndTime;

    bool m_bIsRecvAll;  //6-17，看学生端接收信息是不是发送给全班的
    bool m_bIsSubmit;   //6-21,是否已经回答
};

#endif // ANSWERWINDOW_H

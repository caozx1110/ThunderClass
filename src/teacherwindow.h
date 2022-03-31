#ifndef TEACHERWINDOW_H
#define TEACHERWINDOW_H

#include <QDialog>
#include <QCloseEvent>
#include <QShowEvent>
#include <QTimer>
#include "teacherprocess.h"
#include "questionwindow.h"
#include "activeinfowindow.h"

namespace Ui {
class TeacherWindow;
}

class TeacherWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TeacherWindow(QWidget *parent = nullptr);
    ~TeacherWindow();

private slots:
    void closeEvent (QCloseEvent* e) override;
    void showEvent(QShowEvent* e) override;

    void on_btnBeginClass_clicked();

    void on_ckbScreenShare_clicked();

    void on_ckbVoiceShare_clicked();

    void on_btnEndClass_clicked();

    void on_btnSend_clicked();

    void ClassBegined();

    void ClassEnded();

    void RecvStudentLogin(QString Name);

    void RecvStudentLogout(QString Name);

    void RecvChat(QString Name, QString Msg);

    void RecvAnswer(QString Name, QString Answ);    //6-17，收到答案

    void on_btnSendQues_clicked();                  //6-16，发题

    void on_btnRandQues_clicked();                  //6-16，随机点名

private:
    Ui::TeacherWindow *ui;
    TeacherProcess* m_pProc;            //业务流程类对象指针
    QuestionWindow* m_pQuestionWindow;  //6-16

    QTimer* m_pTimer;
    void UpdateControlEnabled(bool IsInClass);
};

#endif // TEACHERWINDOW_H

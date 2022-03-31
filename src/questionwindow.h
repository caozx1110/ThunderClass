#ifndef QUESTIONWINDOW_H
#define QUESTIONWINDOW_H

#include "teacherprocess.h"
#include <QDialog>
#include <QMessageBox>
#include <QStandardItemModel>

//6-16

namespace Ui {
class QuestionWindow;
}

class QuestionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionWindow(QWidget *parent = nullptr, TeacherProcess* pProc = nullptr, bool IsSendToAll = true);
    ~QuestionWindow();

    void AddAnswer(const QString Name, const QString Ans);

private slots:
    void on_btnQuit_clicked();

    void on_btnSend_clicked();

    void on_btnCount_clicked();

private:
    Ui::QuestionWindow *ui;
    TeacherProcess* m_pProc;    //业务流程类指针
    QStandardItemModel* model;  //6-19，同管理员界面
    vector<pair<QString, QString> > m_ChoiceAnswList; //6-19，存储用户名及其对应的选择题答案
    bool m_bIsSendToAll;   //群发为true, 单发为false
};

#endif // QUESTIONWINDOW_H

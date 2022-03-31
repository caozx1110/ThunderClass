#include "answerwindow.h"
#include "ui_answerwindow.h"

AnswerWindow::AnswerWindow(QWidget *parent, StudentProcess* pProc, bool IsRecvAll, QString Ques, QString OptionA, QString OptionB, QString OptionC, QString OptionD) :
    QDialog(parent),
    ui(new Ui::AnswerWindow)
{
    ui->setupUi(this);
    m_pProc = pProc;
    ui->txtedtQues->setText(Ques);
    m_bIsSubmit = false;

    //6-17，显示选项
    ui->lblOptionA->setText(OptionA);
    ui->lblOptionB->setText(OptionB);
    ui->lblOptionC->setText(OptionC);
    ui->lblOptionD->setText(OptionD);

    //6-17，设定控件的可用状态
    m_bIsRecvAll = IsRecvAll;
    ui->ckbOptA->setEnabled(m_bIsRecvAll);
    ui->ckbOptB->setEnabled(m_bIsRecvAll);
    ui->ckbOptC->setEnabled(m_bIsRecvAll);
    ui->ckbOptD->setEnabled(m_bIsRecvAll);
    ui->txtedtAnswer->setEnabled(!m_bIsRecvAll);

    //6-18，记录开始时间
    m_tBeginTime = time(NULL);
}

AnswerWindow::~AnswerWindow()
{
    delete ui;
}

//6-17更改
void AnswerWindow::on_btnSubmit_clicked()
{
    QMessageBox msgBox;
    time_t UseTime; //6-18

    //6-18，记录结束时间
    m_tEndTime = time(NULL);
    UseTime = m_tEndTime - m_tBeginTime;    //所用时间

    if (m_bIsRecvAll)   //如果是发送给全班，为选择题
    {
        if (ui->ckbOptA->isChecked() || ui->ckbOptB->isChecked() || ui->ckbOptC->isChecked() || ui->ckbOptD->isChecked())   //有选项被选择
        {
            //发送结束则清空输入
            if (m_pProc->SendAnsw(QString::fromStdString((ui->ckbOptA->isChecked() ? "A" : "")) \
                                + QString::fromStdString((ui->ckbOptB->isChecked() ? "B" : "")) \
                                + QString::fromStdString((ui->ckbOptC->isChecked() ? "C" : "")) \
                                + QString::fromStdString((ui->ckbOptD->isChecked() ? "D" : ""))
                                + ", 用时: "
                                + QString::number(UseTime)
                                + "s"))
            {
                //清空所有输入
                ui->txtedtAnswer->clear();
                ui->ckbOptA->setCheckState(Qt::Unchecked);
                ui->ckbOptB->setCheckState(Qt::Unchecked);
                ui->ckbOptC->setCheckState(Qt::Unchecked);
                ui->ckbOptD->setCheckState(Qt::Unchecked);
                m_bIsSubmit = true; //6-21,更改答题状态
                msgBox.setText("回答成功！");
                msgBox.exec();
                this->close();
            }
        }
        else    //没有选项被选择
        {
            msgBox.setText("选择不能为空！");
            msgBox.exec();
        }
    }
    else    //单独随机提问
    {
        if (ui->txtedtAnswer->toPlainText() == "")  //如果没有输入
        {
            msgBox.setText("内容不能为空！");
            msgBox.exec();
        }
        else
        {
            //发送结束则清空聊天输入框
            if (m_pProc->SendAnsw(ui->txtedtAnswer->toPlainText()))
            {
                ui->txtedtAnswer->clear();
                m_bIsSubmit = true; //6-21,更改答题状态
                msgBox.setText("回答成功！");
                msgBox.exec();
                this->close();  //关闭回答界面
            }
        }
    }
}

//无退出按钮，学生必须回答！！！

void AnswerWindow::closeEvent (QCloseEvent* e) {
    //如果没回答，禁止关闭窗口
    if (!m_bIsSubmit)
    {
        e->ignore();
    }
}


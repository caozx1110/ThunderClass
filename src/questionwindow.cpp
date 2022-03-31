#include "questionwindow.h"
#include "ui_questionwindow.h"

//6-16

QuestionWindow::QuestionWindow(QWidget *parent, TeacherProcess* pProc, bool IsSendToAll) :
    QDialog(parent),
    ui(new Ui::QuestionWindow)
{
    ui->setupUi(this);
    m_pProc = pProc;
    m_ChoiceAnswList.clear();   //6-19，清除回答列表

    //6-17，根据是否发送全班设定控件是否可用
    m_bIsSendToAll = IsSendToAll;
    ui->edtQptionA->setEnabled(m_bIsSendToAll);
    ui->edtQptionB->setEnabled(m_bIsSendToAll);
    ui->edtQptionC->setEnabled(m_bIsSendToAll);
    ui->edtQptionD->setEnabled(m_bIsSendToAll);
    ui->btnCount->setEnabled(m_bIsSendToAll);

    //6-19
    model = new QStandardItemModel();
    model->setColumnCount(5);   //列数为5
    //设置表头
    model->setHeaderData(0, Qt::Horizontal, QString::fromStdString("用户名"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromStdString("A"));
    model->setHeaderData(2, Qt::Horizontal, QString::fromStdString("B"));
    model->setHeaderData(3, Qt::Horizontal, QString::fromStdString("C"));
    model->setHeaderData(4, Qt::Horizontal, QString::fromStdString("D"));
    //设定参数
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //使表格不可编辑
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);  //表头居左
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //改变列宽，充满屏幕
}

QuestionWindow::~QuestionWindow()
{
    delete ui;
}

void QuestionWindow::on_btnQuit_clicked()
{
    this->close();  //关闭界面
}

//6-17改为两种情况
void QuestionWindow::on_btnSend_clicked()
{
    QMessageBox MsgBox;
    if (m_bIsSendToAll) //群发
    {
        ui->lblSendToName->setText("全班同学");
        //发送
        m_pProc->SendChoice(ui->txtedtQues->toPlainText(), ui->edtQptionA->text(), ui->edtQptionB->text(), ui->edtQptionC->text(), ui->edtQptionD->text());
    }
    else    //单发
    {
        QString SendToName;
        m_pProc->SendQues(ui->txtedtQues->toPlainText(), SendToName);   //发送

        ui->lblSendToName->setText(SendToName);
    }
    MsgBox.setText("发送成功！");
    MsgBox.exec();
}

//6-17更改显示
//在界面上增加答案
void QuestionWindow::AddAnswer(const QString Name, const QString Ans)
{
    QString text = ui->txtedtAnswer->toPlainText();

    if (m_bIsSendToAll) //如果是群发选择题，储存回答信息
    {
        m_ChoiceAnswList.push_back(pair<QString, QString>(Name, Ans));
    }

    if (text == "") //如果是第一条消息不需要换行
    {
        ui->txtedtAnswer->setText(Name + ": " + Ans);
    }
    else
    {
        ui->txtedtAnswer->setText(text + "\n" + Name + ": " + Ans);
    }
}

//6-19
//统计回答信息
void QuestionWindow::on_btnCount_clicked()
{
    unsigned int i = 0;

    //清空表格
    model->removeRows(0, model->rowCount() - 1);

    //根据回答列表设置表格内容
    for (i = 0; i < m_ChoiceAnswList.size(); i++)
    {
        model->setItem(i, 0, new QStandardItem(m_ChoiceAnswList[i].first));
        //model->item(i, 0)->setTextAlignment(Qt::AlignCenter);           //设置字符位置

        //根据答案信息设定选项的选择与否
        m_ChoiceAnswList[i].second.contains("A") ? model->setItem(i, 1, new QStandardItem("Checked")) : model->setItem(i, 1, new QStandardItem(""));
        m_ChoiceAnswList[i].second.contains("B") ? model->setItem(i, 2, new QStandardItem("Checked")) : model->setItem(i, 2, new QStandardItem(""));
        m_ChoiceAnswList[i].second.contains("C") ? model->setItem(i, 3, new QStandardItem("Checked")) : model->setItem(i, 3, new QStandardItem(""));
        m_ChoiceAnswList[i].second.contains("D") ? model->setItem(i, 4, new QStandardItem("Checked")) : model->setItem(i, 4, new QStandardItem(""));
    }

    //统计某个选项有多少人选择
    model->setItem(i, 0, new QStandardItem("总计"));
    model->item(i, 0)->setForeground(QBrush(QColor(255, 0, 0)));    //设置字符颜色

    //看每一列的选择人数
    for(unsigned int j = 1; j <= 4; j++)
    {
        unsigned int Count = 0;

        for (unsigned int k = 0; k < m_ChoiceAnswList.size(); k++)
        {
            if (model->item(k, j)->text() == "Checked")
            {
                Count++;
            }
        }

        //设置总共的选择人数
        model->setItem(i, j, new QStandardItem(QString::number(Count)));
        model->item(i, j)->setForeground(QBrush(QColor(255, 0, 0)));    //设置字符颜色

    }
}

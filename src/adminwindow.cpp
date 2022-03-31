#include "adminwindow.h"
#include "ui_adminwindow.h"


AdminWindow::AdminWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminWindow)
{
    ui->setupUi(this);
    m_pProc = new AdminProcess();

    //计时
    m_pTimer = new QTimer();
    m_pTimer->setInterval(100);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(TimerEvent()));
    m_pTimer->start();
    //设定窗口最大化，好像没用？
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

    //6-19,圆角输入框
    ui->edtName->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->edtPassword->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->edtChangeName->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->edtDeleteName->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->edtChangedName->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->edtChangedPassword->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");

    //6-15
    model = new QStandardItemModel();
    model->setColumnCount(3);   //三列
    //设置表头
    model->setHeaderData(0, Qt::Horizontal, QString::fromStdString("用户名"));
    model->setHeaderData(1, Qt::Horizontal, QString::fromStdString("密码"));
    model->setHeaderData(2, Qt::Horizontal, QString::fromStdString("用户类型"));
    //设置参数
    ui->tableView->setModel(model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  //使表格不可编辑
    ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);  //表头居左
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //改变列宽，充满屏幕

    /*
    ui->tableView->setColumnWidth(0, 200);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 200);
    */

    //显示信息
    ShowUserInfo();     //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

AdminWindow::~AdminWindow()
{
    m_pTimer->stop();
    delete m_pTimer;
    m_pTimer = nullptr;
    delete m_pProc;
    m_pProc = nullptr;
    delete ui;
}

/*//检查注意力
void AdminWindow::TimerEvent() {
    if(this->isActiveWindow()) {
        ui->label->setText("Active");
    }
    else{
        ui->label->setText("----");
    }
}
*/

//6-14
//展示信息
/*
void AdminWindow::ShowUserInfo()
{
    unsigned int RowCount = m_pProc->GetUserCount();

    //ui->tableWidget->clearContents();   //清空内存，便于更新
    //qDeleteAll(ui->tableWidget->findChildren<QTableWidgetItem*>());
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    //单元格不可编辑

    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("用户名"));
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem("密码"));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem("用户类型"));

    for (unsigned int i = 0; i < RowCount; i++)
    {
        //设置内容
        ui->tableWidget->setItem(i + 1, 0, new QTableWidgetItem(QString::fromStdString(m_pProc->GetNthName(i))));
        ui->tableWidget->setItem(i + 1, 1, new QTableWidgetItem(QString::fromStdString(m_pProc->GetNthPassword(i))));
        ui->tableWidget->setItem(i + 1, 2, new QTableWidgetItem(QString::fromStdString(m_pProc->GetNthType(i))));
    }

}
*/

//6-15
//将信息显示在表格中，刷新
void AdminWindow::ShowUserInfo()
{
    //unsigned int RowCount = model->rowCount();
    //ui->tableView->horizontalHeader()-> setResizeMode(0,QHeaderView::Fixed);
    //ui->tableView->horizontalHeader()-> setResizeMode(1,QHeaderView::Fixed);

    //清空表格
    model->removeRows(0, model->rowCount() - 1);

    unsigned int i = 0;
    string Name;
    string Password;
    string Type;

    //设置单元格内容
    while (m_pProc->GetNthUserInfo(i, Name, Password, Type))
    {
        model->setItem(i, 0, new QStandardItem(QString::fromStdString(Name)));
        //model->item(i, 0)->setTextAlignment(Qt::AlignCenter);           //设置字符位置

        model->setItem(i, 1, new QStandardItem(QString::fromStdString(Password)));
        //model->item(i, 1)->setTextAlignment(Qt::AlignCenter);           //设置字符位置

        model->setItem(i, 2, new QStandardItem(QString::fromStdString(Type)));
        //model->item(i, 2)->setTextAlignment(Qt::AlignCenter);           //设置字符位置
        model->item(i, 2)->setForeground(QBrush(QColor(255, 0, 0)));    //设置字符颜色
        i++;
    }

}


//6-14
//增加用户槽函数
void AdminWindow::on_btnAddUser_clicked()
{
    QMessageBox msgBox;
    string Type;
    if (ui->rbtnTeacher->isChecked())   //被勾选则为教师类
    {
        Type = "Teacher";
    }
    else
        if (ui->rbtnStudent->isChecked())
        {
            Type = "Student";
        }

    if (m_pProc->AddUser(ui->edtName->text().toStdString(), ui->edtPassword->text().toStdString(), Type))   //增加成功
    {
        //清空内容
        ui->edtName->setText("");
        ui->edtPassword->setText("");

        //弹出提示框？
        msgBox.setText("增加成功！");
        msgBox.exec();
    }
    else
    {
        //清空内容
        ui->edtName->setText("");
        ui->edtPassword->setText("");

        //弹出提示框？
        msgBox.setText("增加失败！");
        msgBox.exec();
    }

    //刷新表格
    ShowUserInfo();
}

//6-14
//删除用户
void AdminWindow::on_btnDeleteUser_clicked()
{
    QMessageBox msgBox;

    if (m_pProc->DeleteUser(ui->edtDeleteName->text().toStdString()))   //删除成功
    {
        //清空输入
        ui->edtDeleteName->setText("");

        //弹出提示框？
        msgBox.setText("删除成功！");
        msgBox.exec();
    }
    else
    {
        //清空输入
        ui->edtDeleteName->setText("");

        //弹出提示框？
        msgBox.setText("删除失败！");
        msgBox.exec();
    }

    //刷新表格
    ShowUserInfo();
}

//6-14
//更改用户
void AdminWindow::on_btnChangeUser_clicked()
{
    QMessageBox msgBox;
    string ChangedType;
    if (ui->rbtnTeacher2->isChecked())  //被选择则为老师
    {
        ChangedType = "Teacher";
    }
    else
        if (ui->rbtnStudent->isChecked())
        {
            ChangedType = "Student";
        }

    //更改成功
    if (m_pProc->ChangeUser(ui->edtChangeName->text().toStdString(), ui->edtChangedName->text().toStdString(), ui->edtChangedPassword->text().toStdString(), ChangedType))
    {
        //清空输入
        ui->edtChangeName->setText("");
        ui->edtChangedName->setText("");
        ui->edtChangedPassword->setText("");

        //弹出提示框？
        msgBox.setText("更改成功！");
        msgBox.exec();
    }
    else
    {
        //清空输入
        ui->edtChangeName->setText("");
        ui->edtChangedName->setText("");
        ui->edtChangedPassword->setText("");

        //弹出提示框？
        msgBox.setText("更改失败！");
        msgBox.exec();
    }

    //刷新表格
    ShowUserInfo();
}

//for febug
/*
void AdminWindow::showEvent(QShowEvent* event)
{
    //ShowUserInfo();
}
*/


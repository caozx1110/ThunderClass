#include "loginwindow.h"
#include "ui_loginwindow.h"
#include <QMessageBox>
#include <QImage>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    //6-19,设置圆角输入框、按钮
    ui->edtName->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->edtPassword->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->btnExit->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
    ui->btnLogin->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px");
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_btnLogin_clicked()
{
    QMessageBox msgBox;
    TeacherWindow* pTeacherWindow;
    StudentWindow* pStudentWindow;
    AdminWindow* pAdminWindow;
    LoginResult res = Proc.Login(ui->edtName->text(), ui->edtPassword->text());
    switch (res) {
    case LR_NOUSER :
        msgBox.setText("登录失败");
        msgBox.exec();

        //6-15，如果三次登陆失败，退出登录界面
        if (Proc.IsNoUserThreeTimes())
        {
            this->close();
        }

        break;
    case LR_ADMIN :
        this->setWindowOpacity(0.0);
        pAdminWindow = new AdminWindow(this);
        pAdminWindow->exec();
        this->setWindowOpacity(1.0);
        delete pAdminWindow;
        pAdminWindow = nullptr;
        break;
    case LR_TEACHER :
        this->setWindowOpacity(0.0);
        pTeacherWindow = new TeacherWindow(this);
        pTeacherWindow->setWindowTitle("教师 " + QString(User::GetLoginedUser()->GetName().c_str()));
        pTeacherWindow->exec();
        this->setWindowOpacity(1.0);
        delete  pTeacherWindow;
        pTeacherWindow = nullptr;
        break;
    default:
        this->setWindowOpacity(0.0);
        pStudentWindow = new StudentWindow(this);
        pStudentWindow->exec();
        this->setWindowOpacity(1.0);
        delete pStudentWindow;
        pStudentWindow = nullptr;
    }
}

void LoginWindow::on_btnExit_clicked()
{
    this->close();
}

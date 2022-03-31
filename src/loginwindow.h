#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "teacherwindow.h"
#include "studentwindow.h"
#include "adminwindow.h"
#include "loginprocess.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_btnLogin_clicked();
    void on_btnExit_clicked();

private:
    Ui::LoginWindow *ui;
    LoginProcess Proc;
};
#endif // LOGINWINDOW_H

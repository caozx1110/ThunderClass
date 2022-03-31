#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QString>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QShowEvent>
#include "adminprocess.h"

namespace Ui {
class AdminWindow;
}

class AdminWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();
private slots:
    //void TimerEvent();

    //6-14，添加槽函数
    void ShowUserInfo();
    void on_btnAddUser_clicked();
    void on_btnDeleteUser_clicked();
    void on_btnChangeUser_clicked();

    //void showEvent(QShowEvent* event);

private:
    Ui::AdminWindow *ui;

    //业务流程类对象指针
    AdminProcess* m_pProc;

    //来源网络：https://blog.csdn.net/qq769651718/article/details/79357938
    QStandardItemModel* model;

    QTimer* m_pTimer;
};

#endif // ADMINWINDOW_H

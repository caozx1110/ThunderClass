/*************************************************************************
【文件名】admin.h
【功能模块和目的】管理员类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
#ifndef ADMIN_H
#define ADMIN_H

#include "user.h"
#include "Constant.h"

/*************************************************************************
【类名】Admin
【功能】管理员类
【接口说明】AddUser用于新增一个用户对象，具体功能见相应函数定义处的注释
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
class Admin : public User
{
public:
    //新增用户
    bool AddUser(const string& Name, const string& Password, const string& Type) override;

    //6-14
    //删除一个用户名为Name的用户
    bool DeleteUser(const string &Name) override;
    /*
    //函数GetNthName返回第n个用户名;
    string GetNthName(unsigned int n);
    //函数GetNthPassword返回第n个密码
    string GetNthPassword(unsigned int n);
    //函数GetNthType返回第n个用户类型
    string GetNthType(unsigned int n);
    unsigned int GetUserCount();
    */

    //6-15
    //将第n个用户的信息传给Name，password，Type，返回n是否越界的bool值
    bool GetNthUserInfo(unsigned int n, string& Name, string& Password, string& Type);

private:
    //友元
    friend class User;
    //私有构造函数，不允许在类外显性创建Admin对象
    Admin();

};

#endif // ADMIN_H

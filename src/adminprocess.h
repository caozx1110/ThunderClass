/*************************************************************************
【文件名】adminprocess.h
【功能模块和目的】管理员业务流程类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/

#ifndef ADMINPROCESS_H
#define ADMINPROCESS_H
#include "admin.h"
#include <string>
using namespace  std;

/*************************************************************************
【类名】AdminProcess
【功能】管理员业务流程类
【接口说明】
【开发者及日期】
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
class AdminProcess
{
public:
    AdminProcess();

    //6-14
    //增加用户
    bool AddUser(const string& Name, const string& Password, const string &Type);
    //删除用户
    bool DeleteUser(const string& Name);
    //更改用户
    bool ChangeUser(const string& Name, const string& ChangedName, const string& Password, const string &Type);
    /*
    //函数GetNthName返回第n个用户名;
    string GetNthName(unsigned int n);
    //函数GetNthPassword返回第n个密码
    string GetNthPassword(unsigned int n);
    //函数GetNthType返回第n个用户类型
    string GetNthType(unsigned int n);
    unsigned int GetUserCount();
    */

    //获取第n个用户的信息，用引用传参
    bool GetNthUserInfo(unsigned int n, string& Name, string& Password, string& Type);

private:
    //管理员对象的指针，指向唯一的管理员对象
    Admin* m_pAdmin;
};

#endif // ADMINPROCESS_H

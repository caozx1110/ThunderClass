/*************************************************************************
【文件名】adminprocess.cpp
【功能模块和目的】管理员业务流程类定义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/

#include "adminprocess.h"
#include "admin.h"

/*************************************************************************
【函数名称】AdminProcess::AdminProcess
【函数功能】管理员业务流程类构造函数，不允许在类外显性构造管理员对象（友元User类除外）
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
AdminProcess::AdminProcess() {
    //Admin::LoadFromFile(USERINFOFILENAME); //6-14

    m_pAdmin = (Admin*)Admin::GetUser("Admin");
}

//6-14
/*************************************************************************
【函数名称】AdminProcess::AddUser
【函数功能】增加用户并保存本地文件
【参数】新增用户的姓名、密码、类型
【返回值】增加成功返回true
【开发者及日期】曹展翔，2020-6-14
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
bool AdminProcess::AddUser(const string& Name, const string& Password, const string &Type)
{
    bool flag;
    flag = m_pAdmin->AddUser(Name, Password, Type);

    Admin::SaveToFile(USERINFOFILENAME);

    return flag;
}

//6-14
/*************************************************************************
【函数名称】AdminProcess::DeleteUser
【函数功能】删除用户并储存到文件中
【参数】要删除用户的用户名
【返回值】删除成功返回true
【开发者及日期】曹展翔，2020-6-14
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
bool AdminProcess::DeleteUser(const string& Name)
{
    bool flag;
    flag = m_pAdmin->DeleteUser(Name);

    Admin::SaveToFile(USERINFOFILENAME);

    return flag;
}

//6-14
/*************************************************************************
【函数名称】AdminProcess::ChangeUser
【函数功能】更改一个用户的信息并存到文件
【参数】要更改的用户名，更改后的用户名、密码、类型
【返回值】更改成功返回true
【开发者及日期】曹展翔，2020-6-14
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
bool AdminProcess::ChangeUser(const string& Name, const string& ChangedName, const string& Password, const string &Type)
{
    return (DeleteUser(Name) && AddUser(ChangedName, Password, Type));
}

//6-14
//函数GetName返回第n个用户名;
/*
string AdminProcess::GetNthName(unsigned int n)
{
    return m_pAdmin->GetNthName(n);
}
//函数GetPassword返回第n个密码
string AdminProcess::GetNthPassword(unsigned int n)
{
    return m_pAdmin->GetNthPassword(n);
}
//函数GetType返回第n个用户类型
string AdminProcess::GetNthType(unsigned int n)
{
    return m_pAdmin->GetNthType(n);
}
//返回用户数
unsigned int AdminProcess::GetUserCount()
{
    Admin::LoadFromFile(USERINFOFILENAME);
    return m_pAdmin->GetUserCount();
}
*/

//6-15
/*************************************************************************
【函数名称】AdminProcess::GetNthUserInfo
【函数功能】找到第n个用户，并将数据传递给引用的参数
【参数】第n个用户，需要将数据传递给的三个变量
【返回值】成功找到返回true
【开发者及日期】曹展翔，2020-6-15
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
bool AdminProcess::GetNthUserInfo(unsigned int n, string& Name, string& Password, string& Type)
{
    return m_pAdmin->GetNthUserInfo(n, Name, Password, Type);
}


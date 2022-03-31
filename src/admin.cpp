/*************************************************************************
【文件名】admin.cpp
【功能模块和目的】管理员类定义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/

#include "admin.h"

/*************************************************************************
【函数名称】Admin::Admin
【函数功能】管理员类私有构造函数，不允许在类外显性构造管理员对象（友元User类除外）
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/

Admin::Admin():User("Admin", "Admin", "Admin") {
    //只起到间接调用基类构造函数的作用

}

/*************************************************************************
【函数名称】Admin::AddUser
【函数功能】按用户名、密码、用户类型新增一个用户对象，对象的指针保存在基类User的静态对象vector<User*> UserList中
【参数】三个参数均为输入参数，分别为用户名、密码、类型（Admin、Teacher、Student三者之一）
【返回值】true表示新增成功，false表示新增失败
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
bool Admin::AddUser(const string &Name, const string &Password, const string &Type){

    return User::AddUser(Name, Password, Type);

}

//6-14
/*************************************************************************
【函数名称】Admin::DeleteUser
【函数功能】按用户名删除一个用户对象
【参数】用户名
【返回值】true表示删除成功，false表示删除失败
【开发者及日期】曹展翔，2020-6-14
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
bool Admin::DeleteUser(const string &Name)
{
    return User::DeleteUser(Name);
}

//6-14
/*
//函数GetNthName返回第n个用户名;
string Admin::GetNthName(unsigned int n)
{
    return User::GetNthName(n);
}
//函数GetNthPassword返回第n个密码
string Admin::GetNthPassword(unsigned int n)
{
    return User::GetNthPassword(n);
}
//函数GetNthType返回第n个用户类型
string Admin::GetNthType(unsigned int n)
{
    return  User::GetNthType(n);
}
//返回用户数
unsigned int Admin::GetUserCount()
{
    return User::UserCount;
}
*/


//6-15
/*************************************************************************
【函数名称】Admin::GetNthUserInfo
【函数功能】找到第n个用户，并将用户信息传递给参数
【参数】n为第n个用户，Name，Password，Type均为需要传值的对象
【返回值】成功找到到用户返回true，否则为false
【开发者及日期】曹展翔，2020-6-15
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
bool Admin::GetNthUserInfo(unsigned int n, string& Name, string& Password, string& Type)
{
    return User::GetNthUserInfo(n, Name, Password, Type);
}

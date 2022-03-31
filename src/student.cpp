/*************************************************************************
【文件名】student.cpp
【功能模块和目的】学生类定义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/

#include "student.h"

/*************************************************************************
【函数名称】Student::Student
【函数功能】按用户名、密码构造一个学生对象
【参数】入口参数，分别为用户名、密码
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
Student::Student(const string& Name, const string& Password):User(Name, Password, "Student") {
    //教师端使用，默认没有连接到教师端
    m_iConnectionIndex = -1;
}

/*************************************************************************
【函数名称】Student::IsOnline
【函数功能】用于教师端，判断一个学生是否在线，在Server的哪个Connection上
【参数】出口参数，ConnectionIndex，如果在线，此参数被赋值为该学生在Server上的连接索引（从0开始的）
【返回值】true表示在线，false表示不在线
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
bool Student::IsOnline(int& ConnectionIndex) const {
    if (m_iConnectionIndex >= 0) {
        ConnectionIndex = m_iConnectionIndex;
        return  true;
    }
    return false;
}

/*************************************************************************
【函数名称】Student::Online
【函数功能】用于教师端，设定一个学生在线，以及在Server的哪个Connection上
【参数】入口参数，ConnectionIndex，该学生在Server上的连接索引（从0开始的）
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
void Student::Online(unsigned int ConnectionIndex) {
    m_iConnectionIndex = ConnectionIndex;
}

/*************************************************************************
【函数名称】Student::Offline
【函数功能】用于教师端，设定一个学生离线
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
void Student::Offline(){
    m_iConnectionIndex = -1;
}

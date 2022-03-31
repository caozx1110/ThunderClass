/*************************************************************************
【文件名】student.h
【功能模块和目的】学生类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
#ifndef STUDENT_H
#define STUDENT_H

#include "user.h"

/*************************************************************************
【类名】Student
【功能】学生类
【接口说明】
    构造函数Student(const string& Name, const string& Password);
    函数IsOnline(int& ConnectionIndex)，用于教师端，返回是否学生在线
    函数Online(unsigned int ConnectionIndex)，用于教师端，设定学生在线，并使从Server的ConnectionIndex号连接收发数据
    函数Offline()，用于教师端，设定学生离线
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
class Student : public User {
public:
    //构造函数Student(const string& Name, const string& Password);
    Student(const string& Name, const string& Password);
    //函数IsOnline(int& ConnectionIndex)，用于教师端，返回是否学生在线
    bool IsOnline(int& ConnectionIndex) const;
    //函数Online(unsigned int ConnectionIndex)，用于教师端，设定学生在线，并使从Server的ConnectionIndex号连接收发数据
    void Online(unsigned int ConnectionIndex);
    //函数Offline()，用于教师端，设定学生离线
    void Offline();
private:
    int m_iConnectionIndex;
};

#endif // STUDENT_H

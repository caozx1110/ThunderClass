/*************************************************************************
【文件名】teacher.h
【功能模块和目的】教师类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【版权信息】开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
#ifndef TEACHER_H
#define TEACHER_H

#include "user.h"

/*************************************************************************
【类名】Teacher
【功能】教师类
【接口说明】
    构造函数Teacher(const string& Name, const string& Password);
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】2020-5-15 由范静涛增加注释
*************************************************************************/
class Teacher: public User
{
public:
    Teacher(const string& Name, const string& Password);
};

#endif // TEACHER_H

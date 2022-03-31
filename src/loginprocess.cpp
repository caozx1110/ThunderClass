#include "loginprocess.h"

LoginProcess::LoginProcess()
{
    User::LoadFromFile("UserInformation.txt");
}

//6-15，设置初始登录错误次数为0
unsigned int LoginProcess::m_uNoUserTimes = 0;

LoginResult LoginProcess::Login(const QString& UserName, const QString& Password){
    const User* pCurrentUser = User::GetUser(UserName.toStdString(), Password.toStdString());
    if (pCurrentUser == nullptr) {
        m_uNoUserTimes++;   //6-15，如果不对则错误次数+1
        return LR_NOUSER;
    }
    if (pCurrentUser->Type == "Admin") {
        return LR_ADMIN;
    }
    else if (pCurrentUser->Type == "Teacher") {
        return LR_TEACHER;
    }
    else {
        return LR_STUDENT;
    }
}

//6-15，是否错误三次
bool LoginProcess::IsNoUserThreeTimes()
{
    if (m_uNoUserTimes == 3)    //如果是
    {
        m_uNoUserTimes = 0; //重置
        return true;    //返回真
    }
    else
    {
        return false;
    }
}


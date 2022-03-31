#ifndef LOGINPROCESS_H
#define LOGINPROCESS_H
#include <vector>
#include <string>
#include <QString>
#include "user.h"
using namespace std;

enum LoginResult {LR_NOUSER, LR_ADMIN, LR_TEACHER, LR_STUDENT};

class LoginProcess
{
public:
    LoginProcess();
    LoginResult Login(const QString& UserName, const QString& Password);

    //6-15，增加函数判断是否登陆失败三次
    bool IsNoUserThreeTimes();
private:

    //6-15,增加登陆失败的次数
    static unsigned int m_uNoUserTimes;
};

#endif // LOGINPROCESS_H

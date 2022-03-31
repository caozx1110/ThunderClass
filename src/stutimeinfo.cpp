#include "stutimeinfo.h"

/*************************************************************************
【函数名称】StuTimeInfo::StuTimeInfo
【函数功能】构造函数，初始化赋值
【参数】分别为连接号（int）、（string：）姓名、开始时间、注意力比例、结束时间
【返回值】构造函数不可有返回值
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
StuTimeInfo::StuTimeInfo(unsigned int ConnIdex, string Name, string BeginTime, string ActInfo, string EndTime)
{
    m_uConnIdex = ConnIdex;
    m_sName = Name;
    m_sBeginTime = BeginTime;
    m_sActInfo = ActInfo;
    m_sEndTime = EndTime;
}

/*************************************************************************
【函数名称】StuTimeInfo::~StuTimeInfo
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
StuTimeInfo::~StuTimeInfo()
{

}

/*************************************************************************
【函数名称】StuTimeInfo::StuTimeInfo
【函数功能】拷贝构造函数
【参数】一个该对象的常引用
【返回值】拷贝构造函数不可有返回值
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
StuTimeInfo::StuTimeInfo(const StuTimeInfo& ATimeInfo)
{
    m_uConnIdex = ATimeInfo.m_uConnIdex;
    m_sName = ATimeInfo.m_sName;
    m_sBeginTime = ATimeInfo.m_sBeginTime;
    m_sActInfo = ATimeInfo.m_sActInfo;
    m_sEndTime = ATimeInfo.m_sEndTime;
}

/*************************************************************************
【函数名称】StuTimeInfo::operator=
【函数功能】=重载
【参数】一个该对象的常引用
【返回值】返回一个该类对象的引用
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
StuTimeInfo& StuTimeInfo::operator=(const StuTimeInfo& ATimeInfo)
{
    if (this != &ATimeInfo)
    {
        this->m_uConnIdex = ATimeInfo.m_uConnIdex;
        this->m_sName = ATimeInfo.m_sName;
        this->m_sBeginTime = ATimeInfo.m_sBeginTime;
        this->m_sActInfo = ATimeInfo.m_sActInfo;
        this->m_sEndTime = ATimeInfo.m_sEndTime;
    }

    return *this;
}

/*************************************************************************
【函数名称】StuTimeInfo::SetActInfo
【函数功能】设置注意力数据
【参数】string，注意力数据
【返回值】无
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
void StuTimeInfo::SetActInfo(string ActInfo)
{
    m_sActInfo = ActInfo;
}

/*************************************************************************
【函数名称】StuTimeInfo::SetEndTime
【函数功能】设置结束时间
【参数】string，结束时间
【返回值】无
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
void StuTimeInfo::SetEndTime(string EndTime)
{
    m_sEndTime = EndTime;
}


//以下函数可以改为常引用？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
/*************************************************************************
【函数名称】StuTimeInfo::GetConnIdex
【函数功能】返回端口号数据
【参数】无
【返回值】usigned int，端口号数据
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
unsigned int StuTimeInfo::GetConnIdex()
{
    return m_uConnIdex;
}

/*************************************************************************
【函数名称】StuTimeInfo::GetName
【函数功能】返回用户名
【参数】无
【返回值】string，用户名
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
string StuTimeInfo::GetName()
{
    return m_sName;
}

/*************************************************************************
【函数名称】StuTimeInfo::GetBeginTime
【函数功能】返回开始时间
【参数】无
【返回值】string，开始时间
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
string StuTimeInfo::GetBeginTime()
{
    return m_sBeginTime;
}


/*************************************************************************
【函数名称】StuTimeInfo::GetActInfo
【函数功能】返回注意力数据
【参数】无
【返回值】string，注意力数据
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
string StuTimeInfo::GetActInfo()
{
    return m_sActInfo;
}

/*************************************************************************
【函数名称】StuTimeInfo::GetEndTime
【函数功能】返回结束时间
【参数】无
【返回值】string，结束时间
【开发者及日期】曹展翔，6-18
【更改记录】
*************************************************************************/
string StuTimeInfo::GetEndTime()
{
    return m_sEndTime;
}

/*************************************************************************
【文件名】audioframe.h
【功能模块和目的】音频类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【版权信息】以下代码来源于对CSDN为博主"_寒潭雁影"2篇原创文章源码的修改和封装，、
    开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对修改部分的任何版权诉求，任何使用者可做出任何修改、用于任何目的
    原文遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明
    https://blog.csdn.net/oHanTanYanYing/article/details/31155985
    https://blog.csdn.net/oHanTanYanYing/article/details/29943973
    CC BY-SA 4.0 是一种许可协议，赋予遵守者两项重要权利：
    一、共享的权利。允许通过任何媒介和任何形式复制、发行作品。
    二、改编的权利。允许二次加工、转换和基于作品进行创作，不限制用途，甚至是商业应用。
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改和类结构优化
    2020-05-15 由范静涛增加注释
*************************************************************************/
#include "audioframe.h"

//类静态成员初始化，表示数据区默认大小10k字节
const unsigned int AudioFrame::BUF_SZIE = 1024 * 10;

//类静态成员初始化，表示录音和播放的音频格式
//请注意，这种结构体作为静态成员的初始化形式比较少见，初始化要加.成员名
const WAVEFORMATEX AudioFrame::FORMAT = {
    .wFormatTag = WAVE_FORMAT_PCM, //FORMAT.wFormatTag, 声音格式为PCM
    .nChannels = 1,                //FORMAT.nChannels, 采样声道数，1声道
    .nSamplesPerSec = 8000,        //FORMAT.nSamplesPerSec, 采样率，8000次/秒
    .nAvgBytesPerSec = 8000,       //FORMAT.nAvgBytesPerSec, 每秒的数据率，就是每秒能采集多少字节的数据
    .nBlockAlign = 1,              //FORMAT.nBlockAlign, 一个块的对齐字节数，采样bit的字节数乘以声道数
    .wBitsPerSample = 8,           //FORMAT.wBitsPerSample, 采样比特，16bits/次
    .cbSize = 0                    //FORMAT.cbSize, 一般为0
};

/*************************************************************************
【函数名称】AudioFrame::AudioFrame
【函数功能】构造函数，创建一个头、一个BUF_SIZE大小的数据区，两者关联
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
AudioFrame::AudioFrame(): pBuffer(m_pBuffer), Header(m_Header) {
    m_pBuffer = new BYTE[BUF_SZIE];

    m_Header.lpData = (LPSTR)m_pBuffer;
    m_Header.dwBufferLength = BUF_SZIE;
    m_Header.dwBytesRecorded = 0L;
    m_Header.dwUser = 0L;
    m_Header.dwFlags = 0L;
    m_Header.dwLoops = 1L;
}

/*************************************************************************
【函数名称】AudioFrame::AudioFrame（const BYTE* pInBuffer, unsigned int Length）
【函数功能】构造函数，创建一个头、一个Legnth大小的数据区，两者关联，数据区里的数据被填充为pInBuffer所知空间里的数据
【参数】pInBuffer：输入参数，指向音频数据；Length：输入参数，表示音频数据长度
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
AudioFrame::AudioFrame(const BYTE* pInBuffer, unsigned int Length): pBuffer(m_pBuffer), Header(m_Header) {
    m_pBuffer = new BYTE[Length];
    memcpy(m_pBuffer, pInBuffer, Length);

    m_Header.lpData = (LPSTR)m_pBuffer;
    m_Header.dwBufferLength = Length;
    m_Header.dwBytesRecorded = Length;
    m_Header.dwUser = 0L;
    m_Header.dwFlags = 0L;
    m_Header.dwLoops = 1L;
}

/*************************************************************************
【函数名称】AudioFrame::AudioFrame（const AudioFrame& Frame）
【函数功能】拷贝构造函数，深拷贝
【参数】Frame，输入参数，表示源对象
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
AudioFrame::AudioFrame(const AudioFrame& Frame):pBuffer(m_pBuffer), Header(m_Header) {
    m_Header = Frame.m_Header;
    m_pBuffer = new BYTE[m_Header.dwBufferLength];
    memcpy(m_pBuffer, Frame.m_pBuffer, m_Header.dwBufferLength);
    m_Header.lpData = (LPSTR)m_pBuffer;
}

/*************************************************************************
【函数名称】AudioFrame::operator=
【函数功能】赋值运算符函数，深拷贝
【参数】Frame，输入参数，表示源对象
【返回值】被赋值对象的引用
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
AudioFrame& AudioFrame::operator= (const AudioFrame& Frame) {
    if (this != &Frame) {
        delete[] m_pBuffer;
        m_Header = Frame.m_Header;
        m_pBuffer = new BYTE[m_Header.dwBufferLength];
        memcpy(m_pBuffer, Frame.m_pBuffer, m_Header.dwBufferLength);
        m_Header.lpData = (LPSTR)m_pBuffer;
    }
    return *this;
}

/*************************************************************************
【函数名称】AudioFrame::~AudioFrame
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
AudioFrame::~AudioFrame() {
    delete[] m_pBuffer;
}


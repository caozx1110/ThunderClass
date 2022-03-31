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

#ifndef AUDIOFRAME_H
#define AUDIOFRAME_H

#include <Windows.h>

//如果使用vs，请取消 下一行的注释
//#pragma comment(lib, "winmm.lib")

//用于友元的前置类声明
class Audio;

/*************************************************************************
【类名】AudioFrame
【功能】音频帧类，用于构造、存储一段音频
【接口说明】
    构造函数AudioFrame()构造一个空的音频帧，可容纳数据10k字节
    构造函数AudioFrame(const BYTE* pInBuffer, unsigned int Length)构造特定音频数据的音频帧
    析构~AudioFrame();
    拷贝构造函数AudioFrame(const AudioFrame& Frame);
    赋值运算符AudioFrame& operator= (const AudioFrame& Frame);
    静态常量BUF_SZIE，数据区大小，固定为10k字节
    静态常量结构体FORMAT，采集/播放音频的格式
    数据区指针pBuffer的常引用
    音频头的常引用Header
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
class AudioFrame {
public:
    //构造一个空的音频帧，可容纳数据10k字节
    AudioFrame();
    //构造特定音频数据的音频帧
    AudioFrame(const BYTE* pInBuffer, unsigned int Length);
    //析构函数
    ~AudioFrame();
    //拷贝构造函数
    AudioFrame(const AudioFrame& Frame);
    //赋值运算符
    AudioFrame& operator= (const AudioFrame& Frame);
    //静态常量BUF_SZIE，数据区大小，固定为10k字节
    static const unsigned int BUF_SZIE;
    //静态常量结构体FORMAT，采集/播放音频的格式
    static const WAVEFORMATEX FORMAT;
    //数据区指针pBuffer的常引用
    BYTE* const& pBuffer;
    //音频头的常引用Header
    const WAVEHDR& Header;
private:
    //友元
    friend class Audio;
    //采集/播放音频的格式，结构体
    WAVEHDR m_Header;
    //音频数据区指针
    BYTE* m_pBuffer;
};

#endif // AUDIOFRAME_H

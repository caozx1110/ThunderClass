/*************************************************************************
【文件名】audio.h
【功能模块和目的】音频类声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【版权信息】以下代码来源于对CSDN为博主"_寒潭雁影"2篇原创文章源码的修改和封装
    原版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
    原文连接1：https://blog.csdn.net/oHanTanYanYing/article/details/31155985
    原文连接2：https://blog.csdn.net/oHanTanYanYing/article/details/29943973
    开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对修改部分的任何版权诉求，任何使用者可做出任何修改、用于任何目的
    CC BY-SA 4.0 是一种许可协议，赋予遵守者两项重要权利：
    一、共享的权利。允许通过任何媒介和任何形式复制、发行作品。
    二、改编的权利。允许二次加工、转换和基于作品进行创作，不限制用途，甚至是商业应用。
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
#ifndef AUDIO_H
#define AUDIO_H

#include "audioframe.h"
#include "Constant.h"
#include <queue>
using namespace std;

/*************************************************************************
【类名】Audio
【功能】音频类，用于录制和播放音频队列
【接口说明】
    函数Audio                     构造函数
    函数~Audio                    析构函数
    函数StartRec                  开始录音
    函数StopRec                   停止录音
    函数StartPlay                 开始播放
    函数StopPlay                  停止播放
    函数GetAndRemoveFirstRecorded 获取并删除最早录制的一段音频
    函数AppendFrameToPlay         向待播放队列最后追加一段音频
    函数Clear                     清除全部已录制的音频和待播放的音频
    函数RecordFramesCount         已录制音频队列里的音频数量(单位为秒，整数)
    函数ToPlayFramesCount         待播放音频队列里的音频数量(单位为秒，整数)
    常引用IsRecording              表示是否正在录音
    常引用IsPlaying                表示是否正在播放
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
class Audio {
public:
    //构造函数
    Audio();

    //析构函数
    ~Audio();

    //禁用拷贝构造函数
    Audio(const Audio&) = delete;

    //禁用赋值运算符
    Audio& operator= (const Audio&) = delete;

    //开始录音
    void StartRec();

    //停止录音
    void StopRec();

    //开始播放
    void StartPlay();

    //停止播放
    void StopPlay();

    //获取并删除最早录制的一段音频
    bool GetAndRemoveFirstRecorded(AudioFrame& Frame);

    //向待播放队列最后追加一段音频
    void AppendFrameToPlay(const AudioFrame& Frame);

    //清除全部已录制的音频和待播放的音频
    void Clear();

    //返回已录制音频队列里的音频数量(单位为秒，整数)
    unsigned int RecordFramesCount();

    //待播放音频队列里的音频数量(单位为秒，整数)
    unsigned int ToPlayFramesCount();

    //常引用IsRecording表示是否正在录音
    const bool& IsRecording;

    //常引用IsPlaying表示是否正在播放
    const bool& IsPlaying;
private:
    //录音线程句柄
    HANDLE m_hRecThread;

    //播放线程句柄
    HANDLE m_hPlayThread;

    //录音线程函数
    static DWORD WINAPI RecThread(LPVOID lpParameter);

    //播放线程句柄
    static DWORD WINAPI PlayThread(LPVOID lpParameter);

    //已录制音频队列
    queue<AudioFrame> m_RecordedFrames;

    //待播放音频队列
    queue<AudioFrame> m_ToPlayFrames;

    //是否正在录音
    bool m_bIsRecording;

    //是否正在播放
    bool m_bIsPlaying;
};

#endif // AUDIO_H

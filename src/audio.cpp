/*************************************************************************
【文件名】audio.cpp
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
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/

#include "audio.h"

/*************************************************************************
【函数名称】Audio::Audio
【函数功能】构造函数，仅用于初始化录音和播放状态为false
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Audio::Audio(): IsRecording(m_bIsRecording), IsPlaying(m_bIsPlaying){
    m_bIsRecording = false;
    m_bIsPlaying = false;
}

/*************************************************************************
【函数名称】Audio::~Audio
【函数功能】析构函数，仅用于停止可能正在进行的录音和播放
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Audio::~Audio() {
    StopRec();
    StopPlay();
}

/*************************************************************************
【函数名称】Audio::StartRec
【函数功能】开始录音，启动录音线程
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Audio::StartRec() {
    //已在录音状态则不执行任何实际动作
    if (m_bIsRecording) {
        return;
    }
    //更新为正在录音状态
    m_bIsRecording = true;
    //创建并开始执行录音线程
    m_hRecThread = CreateThread(nullptr, 0, RecThread, (LPVOID)this, 0, nullptr);
    //如创建录音线程失败则，抛出异常。可以改进为抛出标准库异常类型
    if (m_hRecThread == nullptr) {
        throw "Create Record Thread Error";
    }
}

/*************************************************************************
【函数名称】Audio::StopRec
【函数功能】停止录音
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Audio::StopRec() {
    //如不在录音状态则不执行任何实际动作
    if (!m_bIsRecording) {
        return;
    }
    //更新为非录音状态
    m_bIsRecording = false;
    //等待录音线程结束并关闭线程句柄
    WaitForSingleObject(m_hRecThread, WAITMS);
    CloseHandle(m_hRecThread);
    m_hRecThread = nullptr;
}

/*************************************************************************
【函数名称】Audio::StartPlay
【函数功能】开始播放，启动播放线程
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Audio::StartPlay() {
    //如果已是播放状态则不执行任何实际动作
    if (m_bIsPlaying) {
        return;
    }
    //更新为播放状态
    m_bIsPlaying = true;
    //创建并开始执行录音线程
    m_hPlayThread = CreateThread(nullptr, 0, PlayThread, (LPVOID)this, 0, nullptr);
    //如创建播放线程失败则，抛出异常。可以改进为抛出标准库异常类型
    if (m_hPlayThread == nullptr) {
        throw "Create Play Thread Error";
    }
}

/*************************************************************************
【函数名称】Audio::StopPlay
【函数功能】停止播放
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Audio::StopPlay() {
    //如不是播放状态则不执行任何实际操作
    if (!m_bIsPlaying) {
        return;
    }
    //更新为非播放状态
    m_bIsPlaying = false;
    //等待播放线程结束并关闭线程句柄
    WaitForSingleObject(m_hPlayThread, WAITMS);
    CloseHandle(m_hPlayThread);
    m_hPlayThread = nullptr;
}

/*************************************************************************
【函数名称】Audio::RecordFramesCount
【函数功能】返回已录音队列中有多少音频帧
【参数】无
【返回值】已录音队列中音频帧数量，一帧一秒，不可拆分
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
unsigned int Audio::RecordFramesCount() {
    return m_RecordedFrames.size();
}

/*************************************************************************
【函数名称】Audio::ToPlayFramesCount
【函数功能】返回待播放队列中有多少音频帧
【参数】无
【返回值】待播放队列中音频帧数量，一帧一秒，不可拆分
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
unsigned int Audio::ToPlayFramesCount() {
    return m_ToPlayFrames.size();
}

/*************************************************************************
【函数名称】Audio::GetAndRemoveFirstRecorded
【函数功能】判定已录音队列中是否有音频帧，如有则取走并删除最早录制的一个音频帧
【参数】AudioFrame& Frame,出口参数，如已录音队列中有音频帧，则最早录制的一个音频帧将赋值给此参数
【返回值】true表示取走成功，false表示取走失败
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Audio::GetAndRemoveFirstRecorded(AudioFrame& Frame) {
    //队列非空则取走并删除队列头
    if (!m_RecordedFrames.empty()) {
        Frame = m_RecordedFrames.front();
        m_RecordedFrames.pop();
        return true;
    }
    return false;
}

/*************************************************************************
【函数名称】Audio::AppendFrameToPlay
【函数功能】追加一个音频帧到待播放队列尾
【参数】const AudioFrame& Frame, 入口参数，要追加的音频帧
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Audio::AppendFrameToPlay(const AudioFrame& Frame) {
    m_ToPlayFrames.push(Frame);
}

/*************************************************************************
【函数名称】Audio::Clear
【函数功能】清空已录音和待播放队列
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Audio::Clear() {
    //其实还有简单方法：m_RecordedFrames = queue<AudioFrame>();
    while(!m_RecordedFrames.empty()) {
        m_RecordedFrames.pop();
    }
    while (!m_ToPlayFrames.empty()) {
        m_ToPlayFrames.pop();
    }
}

/*************************************************************************
【函数名称】Audio::RecThread
【函数功能】录音线程函数
【参数】lpParameter表示此线程所属的Audio对象
【返回值】无意义，仅仅是函数形式要求
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
DWORD WINAPI Audio::RecThread(LPVOID lpParameter){
    //实参本来就是Audio对象指针，所以此处可以强制转换
    Audio* au = (Audio*)lpParameter;
    //未使用到的事件机制
    HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
    //音频输入设备句柄
    HWAVEIN hWaveIn;
    //新建一个音频帧用于录音
    AudioFrame* Current = new AudioFrame();
    //使用waveInOpen函数开启音频采集
    waveInOpen(&hWaveIn, WAVE_MAPPER, &AudioFrame::FORMAT, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
    //当处于录音状态时，始终执行循环
    while (au-> IsRecording) {
        //准备一个波形数据块头用于录音
        waveInPrepareHeader(hWaveIn, &Current->m_Header, sizeof(WAVEHDR));
        //指定波形数据块为录音输入缓存
        waveInAddBuffer(hWaveIn, &Current->m_Header, sizeof (WAVEHDR));
        //开始录音
        waveInStart(hWaveIn);
        //等待声音录制1s
        Sleep(SLEEPMS);
        //停止录音
        waveInReset(hWaveIn);
        //压入已录音队列
        au->m_RecordedFrames.push(*Current);
        //更换新的一个音频帧准备下次录制循环使用
        delete Current;
        Current = new AudioFrame();
    }
    //释放音频帧
    delete Current;
    //关闭音频输入设备
    waveInClose(hWaveIn);
    return 0;
}

/*************************************************************************
【函数名称】Audio::PlayThread
【函数功能】录音线程函数
【参数】lpParameter表示此线程所属的Audio对象
【返回值】无意义，仅仅是函数形式要求
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-5
【更改记录】
    2020-05-07 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
DWORD WINAPI Audio::PlayThread(LPVOID lpParameter) {
    //实参本来就是Audio对象指针，所以此处可以强制转换
    Audio* au = (Audio*)lpParameter;
     //未使用到的事件机制
    HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
    //音频输出设备句柄
    HWAVEOUT hWaveOut;
    //新建一个音频帧用于播放
    AudioFrame* Current = new AudioFrame();
    //打开一个给定的波形音频输出装置来进行播放
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &AudioFrame::FORMAT, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);
    //当处于播放状态时，始终执行循环
    while(au->IsPlaying) {
        //待播放队列非空，则顺序播放一个音频帧
        if (!au->m_ToPlayFrames.empty()) {
            //用待播放队列头拷贝构造一个要播放的音频帧
            delete Current;
            Current = new AudioFrame(au->m_ToPlayFrames.front());
            //重置音频帧头为未播放状态
            Current->m_Header.dwBufferLength = Current->m_Header.dwBytesRecorded;
            Current->m_Header.dwUser  = 0L;
            Current->m_Header.dwFlags = 0L;
            Current->m_Header.dwLoops = 1L;
            //准备一个波形数据块用于播放
            waveOutPrepareHeader(hWaveOut, &Current->m_Header, sizeof(WAVEHDR));
            //开始播放
            waveOutWrite(hWaveOut, &Current->m_Header, sizeof(WAVEHDR));
            //等待1秒
            Sleep(SLEEPMS);
            //取消波形块的准备状态
            waveOutUnprepareHeader(hWaveOut, &Current->m_Header, sizeof(WAVEHDR));
            //删除待播放队列头的音频帧
            au->m_ToPlayFrames.pop();
        }
    }
    //释放音频帧
    delete Current;
    //关闭音频输出设备
    waveOutClose(hWaveOut);
    return 0;
}

/*************************************************************************
【文件名】Message.h
【功能模块和目的】通用消息类的声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【版权信息】范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对全部代码的任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "audioframe.h"
#include "Image.h"
#include "MessageType.h"
using namespace std;

//预定义一个无符号字符指针类型puchar，方便声明其常引用
typedef unsigned char* puchar;

/*************************************************************************
【类名】Message
【功能】通用消息类的操作
【接口说明】
    构造函数Message();
    构造函数Message(MessageType type, const unsigned char* Body, unsigned int BodySize)指定消息类型和消息内容的
    析构函数virtual ~Message，因为可能是基类，所以析构函数为虚函数
    拷贝构造函数Message(const Message& aMessage);
    赋值运算符Message& operator=(const Message& aMessage);
    函数Type返回消息类型
    函数Clear清除消息内容，重置为空
    函数Append向消息数据区最后追加一段字节序列，并判定是否为一个合理消息
    消息数据区存储了大小（单位字节）
    静态成员函数MakeExitMsg产生一个网络连接断开消息
    静态成员函数MakeIdleMsg产生一个空闲消息
    静态成员函数FromString产生一个文本消息
    静态成员函数FromAudio产生一个音频帧消息
    静态成员函数FromImageJpg产生一个Jpg图像消息
    静态成员函数ToString从文本消息产生一个字符串
    静态成员函数ToAudioFrame从音频消息产生一个音频帧
    静态成员函数ToJpgData从Jpg图像消息产生一个Jpg内存数据块
    静态成员函数ToStudentNameAndPassword从学生登录消息产生用户名和密码
    常引用数据成员Size表示消息占的字节数
    常引用数据成员IsValid表示是否为一个完整的可用的消息
    常引用数据成员Data表示数据区的只读指针
    静态常成员START_BYTE消息的开始符0xFF
    静态常成员STOP_BYTE消息的开始符0xFE
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
    部分参数在设计初期有误，感谢杨宇新和闭浩扬同学指出
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
class Message{
public:
    //构造函数
    Message();

    //指定消息类型和消息内容的构造函数
    Message(MessageType type, const unsigned char* Body, unsigned int BodySize);

    //析构函数，因为可能是基类，所以析构函数为虚函数
    virtual ~Message();

    //拷贝构造函数
    Message(const Message& aMessage);

    //赋值运算符
    Message& operator=(const Message& aMessage);

    //返回消息类型
    MessageType Type() const;

    //清除消息内容，重置为空
    void Clear();    

    //向消息数据区最后追加一段字节序列，并判定是否为一个合理消息
    //Input参数是要追加的字节序列
    //Length参数是要追加的长度
    //RemainLength是追加后Input缓冲区剩余的字节数（追加后不能构成完整消息，则剩余0；如构成完整消息后，还有剩余，则修改remain的值）
    //返回值为ture，表示已经构成完整消息
    bool Append(const unsigned char* Input, unsigned int Length, unsigned int& RemainLength);

    //静态成员函数，构造特定类型的消息
    static Message MakeExitMsg();
    static Message MakeIdleMsg();
    //6-19
    static Message MakeSoonExitMsg();
    static Message FromString(const string& aString);
    //6-17
    static Message FromAnsw(const string& aAnsw);
    //6-18
    static Message FromActProp(const string& aActProp);
    //6-17
    static Message FromChoice(const string& Ques, const string& OptionA, const string& OptionB, const string& OptionC, const string& OptionD);
    static Message FromAudio(const AudioFrame& aFrame);
    static Message FromImageJpg(const Image& anImage);
    //↑还可以自己添加题目、答案、注意力、签到等类型的转换方法

    //静态成员函数，还原消息为特定类型对象
    static string ToString(const Message& Msg);
    static AudioFrame ToAudioFrame(const Message& Msg);
    static void ToStudentNameAndPassword(const Message& Msg, string& Name, string& Password);
    static void ToJpgData(const Message& Msg, char** ppJpgData, unsigned int* JpgSize);
    //6-17
    static void ToChoice(const Message& Msg, string& Ques, string& OptionA, string& OptionB, string& OptionC, string& OptionD);

    //↑还可以自己添加Image、题目、答案、注意力、签到等类型的转换方法

    //消息数据区存储了大小（单位字节）
    const unsigned int& Size;

    //是否是一个头尾完整的消息
    const bool& IsValid;

    //消息数据区的只读指针
    const puchar& Data;

    //静态常成员,消息的开始符和结尾符
    const static unsigned char START_BYTE;
    const static unsigned char STOP_BYTE;

private:
    //将一个字符序列编码成一个消息（加开始符和结尾符号,展开Input中的开始符、结尾符、0x80）
    void Encode(MessageType Type, const unsigned char* Input, unsigned int Length);

    //将一个消息解码为字符序列（去掉消息中的开始符和结尾符号,还原消息原始内容）
    bool Decode(MessageType& Type, unsigned char* Output, unsigned int& Length) const;

    //搜索Buffer中是否包含特定标签（开始符/结尾符）
    bool SearchTag(unsigned char Tag, const unsigned char* Buffer, unsigned int Length, unsigned int& Pos);

    //编码后的消息长度
    unsigned m_uSize;

    //消息数据区
    unsigned char* m_ucData;

    //是否为一个完整消息
    bool m_bIsValid;
};

#endif /* Message_h */

/*************************************************************************
【文件名】Message.cpp
【功能模块和目的】通用消息类的声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【版权信息】范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对全部代码的任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/

#include "Message.h"
#include <stdexcept>

using namespace std;

//消息的开始符和结尾符
const unsigned char Message::START_BYTE = 0xFF;
const unsigned char Message::STOP_BYTE = 0xFE;

/*************************************************************************
【函数名称】Message::Message
【函数功能】构造函数，构造空消息
【参数】无
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message::Message():Size(m_uSize), IsValid(m_bIsValid), Data(m_ucData) {
    m_uSize = 0;
    m_ucData = nullptr;
    m_bIsValid = false;
}

/*************************************************************************
【函数名称】Message::Message(MessageType type, const unsigned char* Body, unsigned int BodySize)
【函数功能】构造函数，通过指定消息类型，未编码的消息体和长度构造指定消息
【参数】全部未入口参数，Type表示消息类型，Body为未编码为消息的原始数据序列，BodySize为原始数据序列长度
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message::Message(MessageType type, const unsigned char* Body, unsigned int BodySize):Size(m_uSize), IsValid(m_bIsValid), Data(m_ucData){
    m_uSize = 0;
    m_ucData = nullptr;
    m_bIsValid = false;
    Encode(type, Body, BodySize);
}

//拷贝构造函数
Message::Message(const Message& aMessage):Size(m_uSize), IsValid(m_bIsValid), Data(m_ucData) {
    m_uSize = aMessage.Size;
    m_ucData = new unsigned char[m_uSize];
    m_bIsValid = aMessage.IsValid;
    memcpy(m_ucData, aMessage.Data, m_uSize);
}

//赋值运算符
Message& Message::operator=(const Message& aMessage){
    if (this != &aMessage){
        //先清空
        Clear();
        m_uSize = aMessage.Size;
        m_ucData = new unsigned char[m_uSize];
        m_bIsValid = aMessage.IsValid;
        memcpy(m_ucData, aMessage.Data, m_uSize);
    }
    return *this;
}

/*************************************************************************
【函数名称】Message::~Message
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message::~Message(){
    Clear();
}

//返回消息类型
MessageType Message::Type() const {
    //不是完整消息不判断类型
    //长度太短也不判定类型
    if (!m_bIsValid || m_uSize < 3) {
        return MSG_UNKOWN;
    }
    //根据m_ucData[1]判断类型，m_ucData[0]是开始符
    if (m_ucData[1] > MSG_UNKOWN && m_ucData[1] < MSG_MAXVALUE) {
        return static_cast<MessageType>(m_ucData[1]);
    }
    else {
        return MSG_UNKOWN;
    }
}

/*************************************************************************
【函数名称】Message::Clear
【函数功能】清空消息数据区
【参数】无
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Message::Clear(){
    //释放消息数据区
    if (m_ucData != nullptr){
        delete [] m_ucData;
        m_ucData = nullptr;
    }
    //不再是完整消息
    m_bIsValid = false;
    //编码后的消息长度为0
    m_uSize = 0;
}

/*************************************************************************
【函数名称】Message::Append
【函数功能】向消息数据区增加一段数据，并判定是否为一个完整的、可用的消息
【参数】
    入口参数，Input参数是要追加的字节序列
    入口参数，Length参数是要追加的长度
    出口参数，RemainLength是追加后Input缓冲区剩余的字节数（追加后不能构成完整消息，则剩余0；如构成完整消息后，还有剩余，则修改remain的值）
【返回值】
    返回值为ture，表示已经构成完整消息
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
    2020-05-19 范静涛在闭浩扬同学建议下优化了逻辑，减少重复搜索次数，降低时间复杂度
*************************************************************************/
bool Message::Append(const unsigned char* Input, unsigned int Length, unsigned int& RemainLength){
    //如果当前消息已经是一个完整的有头有尾的消息了，那么不可再新增加数据
    if (m_bIsValid) {
        RemainLength = Length;
        return m_bIsValid;
    }
    unsigned int MsgStartPos = 0;
    unsigned int MsgStopPos = Length - 1;
    bool IsStartFound = false;
    bool IsStopFound = false;
    //如果当前消息是空的，那么消息头尾搜索范围都在Input里，头之前的都抛弃，尾之后的都留在input里
    if (m_uSize == 0) {
        //搜索头的位置
        IsStartFound = SearchTag(START_BYTE, Input, Length, MsgStartPos);
        //如果找到头，则在头之后去找尾的位置
        if (IsStartFound) {
            IsStopFound = SearchTag(STOP_BYTE, Input + MsgStartPos + 1, Length - MsgStartPos - 1, MsgStopPos);
        }
        //头尾齐全
        if (IsStartFound && IsStopFound) {
            MsgStopPos += (MsgStartPos + 1);
            //从头到尾都放入消息数据里
            m_uSize = MsgStopPos - MsgStartPos + 1;
            m_ucData = new unsigned char[m_uSize];
            memcpy(m_ucData, Input + MsgStartPos, m_uSize);
            //构成合理消息
            m_bIsValid = true;
            //尾之后的留在input中
            RemainLength = Length - MsgStopPos - 1;
            return m_bIsValid;
        }
        //只有头
        else if (IsStartFound){
            //头之后的都放入消息数据里
            m_uSize = Length - MsgStartPos;
            m_ucData = new unsigned char[m_uSize];
            memcpy(m_ucData, Input + MsgStartPos, m_uSize);
            //不是合理消息
            m_bIsValid = false;
            //input里没有剩余数据
            RemainLength = 0;
            return m_bIsValid;
        }
        //头尾都没有
        else {
            m_bIsValid = false;
            //input数据没有剩余数据，都被抛弃了
            RemainLength = 0;
            return m_bIsValid;
        }
    }
    //如果当前消息非空，那么一定有头没有尾，在input中搜索尾即可
    else {
        //在input中搜索尾
        IsStopFound = SearchTag(STOP_BYTE, Input, Length, MsgStopPos);
        //找到尾
        if (IsStopFound){
            //按当前消息长度+尾及之前的长度开辟空间
            unsigned char* pTemp = new unsigned char[m_uSize + MsgStopPos + 1];
            //原来消息和新增部分合成
            memcpy(pTemp, m_ucData, m_uSize);
            memcpy(pTemp + m_uSize, Input, MsgStopPos + 1);
            //释放原来消息数据
            delete[] m_ucData;
            //更新消息长度
            m_uSize  += (MsgStopPos + 1);
            //更新消息数据区指针
            m_ucData = pTemp;
            //构成合理消息
            m_bIsValid = true;
            //尾之后的数据留在input中
            RemainLength = Length - MsgStopPos - 1;
            return m_bIsValid;
        }
        //没有找到尾
        else {
            //input和现有消息数据合并
            unsigned char* pTemp = new unsigned char[m_uSize + Length];
            memcpy(pTemp, m_ucData, m_uSize);
            memcpy(pTemp + m_uSize, Input, Length);
            delete[] m_ucData;
            m_uSize  += Length;
            m_ucData = pTemp;
            //不是合理消息
            m_bIsValid = false;
            RemainLength = 0;
            return m_bIsValid;
        }
    }
}

/*************************************************************************
【函数名称】Message::MakeExitMsg
【函数功能】产生一个断开网络连接的退出消息
【参数】无
【返回值】退出消息对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message Message::MakeExitMsg(){
    return Message(MSG_EXIT, nullptr, 0);
}

/*************************************************************************
【函数名称】Message::MakeIdleMsg
【函数功能】产生一个空闲消息
【参数】无
【返回值】空闲消息对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message Message::MakeIdleMsg() {
    return Message(MSG_IDLE, nullptr, 0);
}

//6-19
/*************************************************************************
【函数名称】Message::MakeSoonExitMsg
【函数功能】制造一个即将下课的消息
【参数】无
【返回值】即将下课消息对象
【开发者及日期】曹展翔，2020-6-19
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
Message Message::MakeSoonExitMsg() {
    return Message(MSG_SOONEXIT, nullptr, 0);
}


/*************************************************************************
【函数名称】Message::FromString
【函数功能】产生一个文本消息
【参数】入口参数，表示文本
【返回值】文本消息对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message Message::FromString(const string& aString) {
    return Message(MSG_STRING, (const unsigned char*)aString.c_str(), aString.length());
}

//6-17
/*************************************************************************
【函数名称】Message::FromAnsw
【函数功能】产生一个回答信息
【参数】入口参数，表示文本
【返回值】答案消息对象
【开发者及日期】曹展翔，2020-6-17
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
Message Message::FromAnsw(const string& aString) {
    return Message(MSG_ANSW, (const unsigned char*)aString.c_str(), aString.length());
}

//6-18
/*************************************************************************
【函数名称】Message::FromActProp
【函数功能】产生一个注意力比例信息
【参数】入口参数，表示文本
【返回值】注意力比例消息对象
【开发者及日期】曹展翔，2020-6-17
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
Message Message::FromActProp(const string& aString) {
    return Message(MSG_ACTPROP, (const unsigned char*)aString.c_str(), aString.length());
}

//6-17
/*************************************************************************
【函数名称】Message::FromChoice
【函数功能】产生一个选择题信息
【参数】入口参数，依次表示题干加四个选项
【返回值】选择题消息对象
【开发者及日期】曹展翔，2020-6-17
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
Message Message::FromChoice(const string& Ques, const string& OptionA, const string& OptionB, const string& OptionC, const string& OptionD)
{
    //记下length，
    unsigned int QuesLen = Ques.length();
    unsigned int OptALen = OptionA.length();
    unsigned int OptBLen = OptionB.length();
    unsigned int OptCLen = OptionC.length();
    unsigned int OptDLen = OptionD.length();
    unsigned int TotalLenth = 5 * sizeof(unsigned int) + QuesLen + OptALen + OptBLen + OptCLen + OptDLen;   //总长度
    unsigned char* pBody = new unsigned char[TotalLenth];

    //拷贝内存，地址偏移，转换为字符串指针
    memcpy((void*)pBody, (void*)(&QuesLen), sizeof(unsigned int));
    memcpy((void*)(pBody + 1 * sizeof(unsigned int)), (void*)(Ques.c_str()), QuesLen);

    memcpy((void*)(pBody + 1 * sizeof(unsigned int) + QuesLen), (void*)(&OptALen), sizeof(unsigned int));
    memcpy((void*)(pBody + 2 * sizeof(unsigned int) + QuesLen), (void*)(OptionA.c_str()), OptALen);

    memcpy((void*)(pBody + 2 * sizeof(unsigned int) + QuesLen + OptALen), (void*)(&OptBLen), sizeof(unsigned int));
    memcpy((void*)(pBody + 3 * sizeof(unsigned int) + QuesLen + OptALen), (void*)(OptionB.c_str()), OptBLen);

    memcpy((void*)(pBody + 3 * sizeof(unsigned int) + QuesLen + OptALen + OptBLen), (void*)(&OptCLen), sizeof(unsigned int));
    memcpy((void*)(pBody + 4 * sizeof(unsigned int) + QuesLen + OptALen + OptBLen), (void*)(OptionC.c_str()), OptCLen);

    memcpy((void*)(pBody + 4 * sizeof(unsigned int) + QuesLen + OptALen + OptBLen + OptCLen), (void*)(&OptDLen), sizeof(unsigned int));
    memcpy((void*)(pBody + 5 * sizeof(unsigned int) + QuesLen + OptALen + OptBLen + OptCLen), (void*)(OptionD.c_str()), OptDLen);

    //构造消息类对象
    Message AMessage(MSG_CHOICE, pBody, TotalLenth);
    delete [] pBody;
    pBody = nullptr;
    return AMessage;
}


/*************************************************************************
【函数名称】Message::FromAudioFrame
【函数功能】产生一个音频帧消息
【参数】入口参数，表示音频帧
【返回值】音频消息对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message Message::FromAudio(const AudioFrame& aFrame) {
    return Message(MSG_AUDIO, aFrame.pBuffer, aFrame.Header.dwBytesRecorded);
}

/*************************************************************************
【函数名称】Message::FromJpgImage
【函数功能】产生一个jpg图像
【参数】入口参数，表示要压缩为jpg的Image对象
【返回值】jpg图图像消息对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Message Message::FromImageJpg(const Image& anImage) {
    char* JpgData = nullptr;
    unsigned int JpgSize = 0;
    //压缩为jpg数据区
    anImage.SaveToJpgData(&JpgData, &JpgSize);
    //用jpg数据区和大小构造消息
    Message ret = Message(MSG_JPG, (unsigned char*)JpgData, JpgSize);
    delete [] JpgData;
    return ret;
}

/*************************************************************************
【函数名称】Message::ToString
【函数功能】从文本消息还原一个文本对象
【参数】入口参数，要还原的消息
【返回值】文本对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
string Message::ToString(const Message& Msg){
    //开辟临时缓冲区
    unsigned char* pBuffer = new unsigned char[Msg.Size];
    //消息类型和解码后长度临时变量
    MessageType type;
    unsigned int length;
    //执行解码
    bool IsDecoded = Msg.Decode(type, pBuffer, length);
    //解码失败或者不是string消息，抛出异常
    if (!IsDecoded || (Msg.Type() != MSG_STRING && Msg.Type() != MSG_QUES && Msg.Type() != MSG_ANSW && Msg.Type() != MSG_ACTPROP))//6-17添加,6-18再添加
    {
        delete[] pBuffer;
        throw(invalid_argument("Message type is not MSG_STRING"));
    }
    //形成文本
    string res = string((char*)(pBuffer), length);
    //释放临时缓冲区
    delete[] pBuffer;
    return res;
}

/*************************************************************************
【函数名称】Message::ToAudioFrame
【函数功能】从音频消息还原一个音频帧对象
【参数】入口参数，要还原的消息
【返回值】音频帧对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
AudioFrame Message::ToAudioFrame(const Message& Msg) {
    //开辟临时缓冲区
    unsigned char* pBuffer = new unsigned char[Msg.Size];
    //消息类型和解码后长度临时变量
    MessageType type;
    unsigned int length;
    //执行解码
    bool IsDecoded = Msg.Decode(type, pBuffer, length);

    //解码失败或者不是音频消息，抛出异常
    if (!IsDecoded || Msg.Type() != MSG_AUDIO) {
        delete[] pBuffer;
        throw(invalid_argument("Message type is not MSG_AUDIO"));
    }
    //形成音频
    AudioFrame res = AudioFrame(pBuffer, length);
    //释放临时缓冲区
    delete[] pBuffer;
    return res;
}

/*************************************************************************
【函数名称】Message::ToJpgData
【函数功能】从jpg图像消息还原一个jpg内存出具看
【参数】入口参数，Msg要还原的消息；出口参数，ppJpgData存放数据区指针的指针变量；出口参数，pJpgSize指向jpg数据区长度的指针变量
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【特别说明】*ppJpgData需要在调用此函数后手动释放，否则会内存泄漏
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Message::ToJpgData(const Message& Msg, char** ppJpgData, unsigned int* pJpgSize){
    //开辟临时缓冲区
    unsigned char* pBuffer = new unsigned char[Msg.Size];
    //消息类型和解码后长度临时变量
    MessageType type;
    unsigned int length;
    //执行解码
    bool IsDecoded = Msg.Decode(type, pBuffer, length);
    //解码失败或者不是Jpg消息，抛出异常
    if (!IsDecoded || Msg.Type() != MSG_JPG) {
        delete[] pBuffer;
        throw(invalid_argument("Message type is not MSG_JPG"));
    }
    *pJpgSize = length;
    *ppJpgData = new char[*pJpgSize];
    memcpy(*ppJpgData, pBuffer, *pJpgSize);
    delete[] pBuffer;
}

/*************************************************************************
【函数名称】Message::ToStudentNameAndPassword
【函数功能】从学生登录消息还原用户名密码
【参数】入口参数，Msg要还原的消息；出口参数，Name用户名；出口参数，Password密码
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Message::ToStudentNameAndPassword(const Message& Msg, string& Name, string& Password){
    //开辟临时缓冲区
    unsigned char* pBuffer = new unsigned char[Msg.Size];
    //消息类型和解码后长度临时变量
    MessageType type;
    unsigned int length;
    //执行解码
    bool IsDecoded = Msg.Decode(type, pBuffer, length);
    //解码失败或者不是用户名密码消息，抛出异常
    if (!IsDecoded || Msg.Type() != MSG_ID_PSW) {
        delete[] pBuffer;
        throw(invalid_argument("Message type is not MSG_ID_PSW"));
    }
    unsigned int LenName = 0;
    memcpy((char*)&LenName, pBuffer, sizeof(LenName));
    Name = string((char*)pBuffer + sizeof(LenName), LenName);
    unsigned int LenPassword;
    memcpy((char*)&LenPassword, pBuffer + sizeof(LenName) + LenName, sizeof(LenPassword));
    Password = string((char*)pBuffer + 2 * sizeof(LenName) + LenName, LenPassword);
    //释放临时缓冲区
    delete[] pBuffer;
}

//6-17
/*************************************************************************
【函数名称】Message::ToChoice
【函数功能】从选择题消息还原题干和选项
【参数】入口参数，分别表示待转换的消息类对象，然后是需要传值的题干和四个选项的引用
【返回值】无
【开发者及日期】曹展翔，2020-6-17
【更改记录】2020-6-21 由曹展翔增加注释
*************************************************************************/
void Message::ToChoice(const Message& Msg, string& Ques, string& OptionA, string& OptionB, string& OptionC, string& OptionD)
{
    //开辟临时缓冲区
    unsigned char* pBuffer = new unsigned char[Msg.Size];
    //消息类型和解码后长度临时变量
    MessageType type;
    unsigned int length;
    //执行解码
    bool IsDecoded = Msg.Decode(type, pBuffer, length);
    //解码失败或者不是用户名密码消息，抛出异常
    if (!IsDecoded || Msg.Type() != MSG_CHOICE) {
        delete[] pBuffer;
        throw(invalid_argument("Message type is not MSG_CHOICE"));
    }

    //解码
    unsigned int QuesLen = 0;
    memcpy((char*)&QuesLen, pBuffer, sizeof(QuesLen));
    Ques = string((char*)pBuffer + sizeof(QuesLen), QuesLen);

    unsigned int OptALen = 0;
    memcpy((char*)&OptALen, pBuffer + 1 * sizeof(QuesLen) + QuesLen, sizeof(OptALen));
    OptionA = string((char*)pBuffer + 2 * sizeof(QuesLen) + QuesLen, OptALen);

    unsigned int OptBLen = 0;
    memcpy((char*)&OptBLen, pBuffer + 2 * sizeof(QuesLen) + QuesLen + OptALen, sizeof(OptBLen));
    OptionB = string((char*)pBuffer + 3 * sizeof(QuesLen) + QuesLen + OptALen, OptBLen);

    unsigned int OptCLen = 0;
    memcpy((char*)&OptCLen, pBuffer + 3 * sizeof(QuesLen) + QuesLen + OptALen + OptBLen, sizeof(OptCLen));
    OptionC = string((char*)pBuffer + 4 * sizeof(QuesLen) + QuesLen + OptALen + OptBLen, OptCLen);

    unsigned int OptDLen = 0;
    memcpy((char*)&OptDLen, pBuffer + 4 * sizeof(QuesLen) + QuesLen + OptALen + OptBLen + OptCLen, sizeof(OptDLen));
    OptionD = string((char*)pBuffer + 5 * sizeof(QuesLen) + QuesLen + OptALen + OptBLen + OptCLen, OptDLen);

    //释放临时缓冲区
    delete[] pBuffer;
}


/*************************************************************************
【函数名称】Message::Encode
【函数功能】将任意数据区编码为特定类型的消息
【参数】全部为入口参数，Type为消息类型，Input为待编码数据区，Length为待编码数据区长度
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Message::Encode(MessageType Type, const unsigned char* Input, unsigned int Length){
    //计算临时缓冲区最大需要长度，每个Input中的1字节都展开为2个字节（0x80或开始符或结尾符号）,加上开始和结尾符2字节，加上Type的1个字节
    unsigned int TempSize = 2 * Length + 2 + 1;
    //开辟临时缓冲区，并用0填充
    unsigned char* TempData = new unsigned char[TempSize];
    memset(TempData, 0, TempSize);
    //放置开始符和消息类型
    TempData[0] = START_BYTE;
    TempData[1] = Type;
    //接下来从缓冲区第2字节开始放置编码后的数据(j也可以认为是已经编码完的消息字节数)
    unsigned int j = 2;
    //输入不为空，长度不为零才能编码
    if (!(Input == nullptr || Length == 0)) {
        //遍历输入原始数据的每一个字节
        for (unsigned int i = 0; i < Length; i++) {
            //是开始符、结尾符、0x80，则展开为2字节，放入临时缓冲区
            if (Input[i] == START_BYTE || Input[i] == STOP_BYTE || Input[i] == 0x80) {
                TempData[j] = 0x80;
                j++;
                TempData[j] = Input[i] - 0x80;
                j++;
            }
            //其他字符原样放入临时缓冲区
            else {
                TempData[j] = Input[i];
                j++;
            }
        }

    }
    //添加结尾符
    TempData[j] = STOP_BYTE;
    j++;
    //清空消息数据区
    Clear();

    //消息长度为编码后获得的字节数
    m_uSize = j;
    //开辟消息数据区
    m_ucData = new unsigned char[m_uSize];
    //编码后数据放入消息数据区
    memcpy(m_ucData, TempData, m_uSize);
    //已经是一个完整消息
    m_bIsValid = true;
    //释放临时缓冲区
    delete[] TempData;
}

/*************************************************************************
【函数名称】Message::Dencode
【函数功能】将消息解码为类型、解码后的数据区、解码后的数据区长度
【参数】全部为出口参数，Type为消息类型，Output为解码后的数据区，Length为解码后数据区长度
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Message::Decode(MessageType& Type, unsigned char* Output, unsigned int& Length) const{
    //不是完整消息不能解码
    if (!this->IsValid) {
        return false;
    }
    //获得消息类型
    Type = this->Type();
    //从Output的第0个位置开始放置解码后的原始数据
    unsigned int j = 0;
    //遍历除了头、类型、尾之外的全部消息字节序列
    for (unsigned int i = 2; i < m_uSize - 1; i++) {
        //遇到0x80就和后续字符合并，放入解码后的原始数据序列
        if (m_ucData[i] == 0x80) {
            Output[j] = m_ucData[i] + m_ucData[i + 1];
            j++;
            i++;
        }
        //其他，原样放入解码后的原始数据序列
        else {
            Output[j] = m_ucData[i];
            j++;
        }
    }
    //设定解码后的原始数据序列有多长
    Length = j;
    return true;
}

/*************************************************************************
【函数名称】Message::SearchTag
【函数功能】在消息数据区中搜索特定的Tag(消息头0xFF或消息尾0xFE)
【参数】
    入口参数，Tag表示要搜索的特定Tag(消息头0xFF或消息尾0xFE)
    入口参数，Buffer要搜索的区域指针
    入口参数，Length要搜索的长度
    出口参数，Pos找到的位置，但返回值为false是此参数无效
【返回值】true表示找到了，false表示未找到
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
bool Message::SearchTag(unsigned char Tag, const unsigned char* Buffer, unsigned int Length, unsigned int& Pos){
    //长度小于1字节，不可能找到Tag
    if (Length == 0) {
        Pos = Length + 1;
        return false;
    }
    //长度为1，只判断第0字节
    if(Length == 1){
        if (Buffer[0] == Tag) {
            Pos = 0;
            return true;
        }
        else {
            Pos = Length ;
            return false;
        }
    }
    //搜索Tag
    Pos = 0;
    while (Pos < Length){
        if (Buffer[Pos] == Tag){
            return true;
        }
        else {
            Pos++;
        }
    }
    Pos = Length;
    return false;
}

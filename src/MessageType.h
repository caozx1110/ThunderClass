/*************************************************************************
【文件名】MessageType.h
【功能模块和目的】通用消息类型标识枚举类型的定义
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【版权信息】范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对全部代码的任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

enum MessageType {
    MSG_UNKOWN,   //未知消息类型
    MSG_ID_PSW,   //用户名密码消息类型
    MSG_EXIT,     //退出消息
    MSG_IDLE,     //空闲消息
    MSG_STRING,   //文本消息
    MSG_SCR_SIZE, //共享屏幕尺寸消息
    MSG_JPG,      //用JPG更新共享屏幕的消息
    MSG_SCR_COPY, //用覆盖法更新共享屏幕的消息
    MSG_SCR_XOR,  //用异或法更新共享屏幕的消息
    MSG_AUDIO,    //音频消息

    //新增类型
    MSG_QUES,      //问题消息（纯文字）
    MSG_ANSW,      //回答消息
    MSG_CHOICE,    //选择题消息
    MSG_ACTPROP,   //注意力比例消息
    MSG_SOONEXIT,  //即将下课的消息

    MSG_MAXVALUE  //一定放在最后一个，防止增删类型导致代码错误
};
#endif // MESSAGETYPE_H

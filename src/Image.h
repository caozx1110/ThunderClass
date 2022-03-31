/*************************************************************************
【文件名】Image.h
【功能模块和目的】图像类的声明
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【版权信息】
    1、屏幕捕获代码来源于对CSDN为博主"_寒潭雁影"2篇原创文章源码的修改和封装
    原版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
    原文链接：https://blog.csdn.net/Reversess/article/details/27402893
    开发者范静涛(fanjingtao@tsinghua.edu.cn)声明放弃对修改部分的任何版权诉求，任何使用者可做出任何修改、用于任何目的
    CC BY-SA 4.0 是一种许可协议，赋予遵守者两项重要权利：
        A. 共享的权利。允许通过任何媒介和任何形式复制、发行作品。
        B. 改编的权利。允许二次加工、转换和基于作品进行创作，不限制用途，甚至是商业应用。
    2.其余代码均为范静涛(fanjingtao@tsinghua.edu.cn)原创，声明放弃任何版权诉求，任何使用者可做出任何修改、用于任何目的
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和结构功能优化，添加了存储为jpg和导出jpg数据的功能
    2020-05-15 由范静涛增加注释
    2020-05-21 由仝牧坤同学发现此头文件不能正常引用的问题，原因是需要前置两个头文件，已更正
*************************************************************************/
#ifndef ImageH
#define ImageH

#include <vector>
#include <string>
//仝牧坤发现必须include以下两个头文件
#include <windows.h>
#include <wingdi.h>
#include <gdiplus.h> //gdiplus是windows操作系统自带功能集合之一，用于图像操作等，是jpg功能的来源
#include <comdef.h>  //与gidplus配合使用
#include "Pixel.h"
using namespace std;

/*************************************************************************
【类名】Image
【功能】操作图像对象
【接口说明】
    构造函数Image构造指定宽高或默认100像素×100像素的全黑色
    析构函数~Image
    拷贝构造函数Image(const Image& anImage);
    赋值运算符函数Image& operator= (const Image& anImage);
    异或运算符函数Image operator^ (const Image& anImage);
    函数ScanLine返回指定行的原始数据区指针
    函数GetPixel返回指定位置的像素值
    函数SetPixel设定指定位置的像素值
    函数LoadFromFile读取本类自定义的图像格式文件
    函数SaveToFile保存图像到本类自定义的图像格式文件
    函数SaveToJpgFile保存图像到Jpg格式图像文件
    函数SaveToJpgData保存图像到Jpg原始数据到指定内存区域
    友元函数CapScreenTo(Image& img)抓取当前屏幕到指定图像对象
    常引用数据成员Width表示以像素为单位的图像宽度
    常引用数据成员Height表示以像素为单位的高度
    常引用数据成员BytesPerLine表示每行图像占了多少字节（4字节整数倍）
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改和结构功能优化，添加了存储为jpg和导出jpg数据的功能
    2020-05-15 由范静涛增加注释
*************************************************************************/
class Image{
public:
    //构造函数Image构造指定宽高或默认100像素×100像素的全黑色
    Image(unsigned int W = 100, unsigned int H = 100);

    //析构函数~Image
    ~Image();

    //拷贝构造函数Image(const Image& anImage);
    Image(const Image& anImage);

    //赋值运算符函数Image& operator= (const Image& anImage);
    Image& operator= (const Image& anImage);

    //异或运算符函数Image operator^ (const Image& anImage);
    Image operator^ (const Image& anImage) const;

    //函数ScanLine返回指定行的原始数据区指针
    unsigned char* ScanLine(unsigned int y);

    //函数GetPixel返回指定位置的像素值
    Pixel GetPixel(unsigned int x, unsigned int y) const;

    //函数SetPixel设定指定位置的像素值
    void SetPixel(unsigned int x, unsigned int y, const Pixel& aPixel);

    //函数LoadFromFile读取本类自定义的图像格式文件
    void LoadFromFile(const string& FileName);

    //函数SaveToFile保存图像到本类自定义的图像格式文件
    void SaveToFile(const string& FileName) const;

    //函数SaveToJpgFile保存图像到Jpg格式图像文件
    void SaveToJpgFile(const string& FileName) const;

    //函数SaveToJpgData保存图像到Jpg原始数据到指定内存区域
    void SaveToJpgData(char** ppBuffer, unsigned int* puLength) const;

    //友元函数CapScreenTo(Image& img)抓取当前屏幕到指定图像对象
    friend void CapScreenTo(Image& img);

    //常引用数据成员Width表示以像素为单位的图像宽度
    const unsigned int& Width;

    //常引用数据成员Height表示以像素为单位的高度
    const unsigned int& Height;

    //常引用数据成员BytesPerLine表示每行图像占了多少字节（4字节整数倍）
    const unsigned int& BytesPerLine;

private:
    //获取Jpg编码器
    int GetJpgEncoderClassID(CLSID* pClassID) const;

    //静态私有成员，一共有多少个Image的对象
    static unsigned int m_uImageCount;

    //图像宽度
    unsigned int m_uiWidth;

    //图像高度
    unsigned int m_uiHeight;

    //用一维数组模拟二维存储结构
    unsigned char* m_ucData;

    //每行像素占用多少字节（4字节整数倍）
    unsigned int m_uiBytePerLine;

    //GDI+ 初始化和结束用的，不用理解
    Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;
};
#endif

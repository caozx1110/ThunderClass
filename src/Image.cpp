/*************************************************************************
【文件名】Image.cpp
【功能模块和目的】图像类的定义
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
*************************************************************************/

#include <fstream>
#include <windows.h>
#include <wingdi.h>
#include <cstring>
#include "Image.h"

//使用vs请取消下行注释
//#pragma comment(lib, "gdiplus")

//Image类私有静态计数器，表示当前一共有多少个Image对象，用于GDI+初始化和释放
unsigned int Image::m_uImageCount = 0;

/*************************************************************************
【函数名称】Image::Image
【函数功能】构造函数，构造指定宽高的全黑色图像
【参数】全部为入口参数，依次为像素为单位的图像宽度和高度，默认参数值均为100
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Image::Image(unsigned int W, unsigned int H) : Width(m_uiWidth), Height(m_uiHeight), BytesPerLine(m_uiBytePerLine) {
    //不合理的宽度或高度，抛出异常
    if (W == 0 || H == 0) {
        throw "Wrong Size";
    }
    m_uiWidth = W;
    m_uiHeight = H;
    //window要求在内存中一行图像必须占用4字节整数倍空间
    m_uiBytePerLine = m_uiWidth * 3;
    if (m_uiBytePerLine % 4 != 0) {
        m_uiBytePerLine += (4 - m_uiBytePerLine % 4);
    }
    //开辟数据区并设置为黑色（全为0）
    m_ucData = new unsigned char[m_uiBytePerLine * m_uiHeight];
    memset(m_ucData, 0, m_uiBytePerLine * m_uiHeight);
    //创建第一个图像时，初始化GDI+
    if (m_uImageCount == 0) {
        GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
    }
    //计数器自增
    m_uImageCount++;
}

/*************************************************************************
【函数名称】Image::~Image
【函数功能】析构函数
【参数】无
【返回值】析构函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Image::~Image() {
    //释放数据区
    delete[] m_ucData;
    //计数器自减
    m_uImageCount--;
    //最后一个图像对象析构，关闭GDI+
    if (m_uImageCount == 0) {
        Gdiplus::GdiplusShutdown( m_gdiplusToken);
    }
}

/*************************************************************************
【函数名称】Image::Image(const Image& anImage)
【函数功能】拷贝函数
【参数】入口参数anImage，表示源图像对象
【返回值】构造函数不可有返回值
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Image::Image(const Image& anImage) : Width(m_uiWidth), Height(m_uiHeight), BytesPerLine(m_uiBytePerLine) {
    m_uiWidth = anImage.m_uiWidth;
    m_uiHeight = anImage.m_uiHeight;
    m_uiBytePerLine = anImage.m_uiBytePerLine;
    m_ucData = new unsigned char[m_uiBytePerLine * m_uiHeight];
    memcpy(m_ucData, anImage.m_ucData, m_uiBytePerLine * m_uiHeight);
    //计数器自减
    m_uImageCount++;
}

/*************************************************************************
【函数名称】Image::opertor=(const Image& anImage)
【函数功能】赋值运算符函数
【参数】入口参数anImage，表示源图像对象
【返回值】被赋值对象的引用
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Image& Image::operator = (const Image & anImage) {
    if (this != &anImage) {
        delete[] m_ucData;
        m_uiWidth = anImage.m_uiWidth;
        m_uiHeight = anImage.m_uiHeight;
        m_uiBytePerLine = anImage.m_uiBytePerLine;
        m_ucData = new unsigned char[m_uiBytePerLine * m_uiHeight];
        memcpy(m_ucData, anImage.m_ucData, m_uiBytePerLine * m_uiHeight);
    }
    return *this;
}

/*************************************************************************
【函数名称】Image::opertor^(const Image& anImage)
【函数功能】异或运算符函数
【参数】入口参数anImage，表示参与异或的第二个图像对象
【返回值】异或结果产生的新图像对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10 取消此函数注释
【更改记录】
    2020-05-15 由范静涛增加注释
*************************************************************************/
Image Image::operator^ (const Image& anImage) const {
    //两幅图像尺寸不一致，无法异或，抛出异常
    if (m_uiWidth  != anImage.m_uiWidth || m_uiHeight != anImage.m_uiHeight) {
        throw "Different size";
    }
    //以当前对象为源构造新图像对象
    Image Ret(*this);
    //新图像数据区与anImage数据区逐字节异或，结果放入新图像数据区
    for (unsigned int idx = 0 ; idx < m_uiBytePerLine * m_uiHeight; idx++) {
        Ret.m_ucData[idx] ^= anImage.m_ucData[idx];
    }
    //返回异或后的新图像对象
    return Ret;
}

/*************************************************************************
【函数名称】Image::ScanLine
【函数功能】返回指定行的数据区指针
【参数】入口参数表示行号，最上行为0行
【返回值】指定行的数据区指针
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
unsigned char* Image::ScanLine(unsigned int y){
    //超范围行号，抛出异常
    if (y >= Height) {
        throw "Out of range";
    }
    return m_ucData + m_uiBytePerLine * y;
}

/*************************************************************************
【函数名称】Image::GetPixel
【函数功能】返回指定位置的像素对象
【参数】入口参数，依次为x坐标和y坐标
【返回值】指定位置的像素对象
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
Pixel Image::GetPixel(unsigned int x, unsigned int y) const {
    //坐标值超范围，抛出异常
    if (x >= Width || y >= Height) {
        throw "Out of range";
    }
    return Pixel(
        m_ucData[m_uiBytePerLine * y + 3 * x + 2],
        m_ucData[m_uiBytePerLine * y + 3 * x + 1],
        m_ucData[m_uiBytePerLine * y + 3 * x + 0]
    );
}

/*************************************************************************
【函数名称】Image::SetPixel
【函数功能】设定指定位置的像素对象
【参数】入口参数，依次为x坐标、y坐标和像素对象
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Image::SetPixel(unsigned int x, unsigned int y, const Pixel& aPixel) {
    //坐标值超范围，抛出异常
    if (x >= Width || y >= Height) {
        throw "Out of range";
    }
    m_ucData[m_uiBytePerLine * y + 3 * x + 2] = aPixel.R;
    m_ucData[m_uiBytePerLine * y + 3 * x + 1] = aPixel.G;
    m_ucData[m_uiBytePerLine * y + 3 * x + 0] = aPixel.B;
}

/*************************************************************************
【函数名称】Image::LoadFromFile
【函数功能】读取本类自定义格式文件到图像对象
【参数】入口参数，文件名字符串
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Image::LoadFromFile(const string& FileName) {
    //打开文件
    ifstream inFile(FileName.c_str(), ios::in | ios::binary);
    if (!inFile) {
        throw "Cann't open file";
    }

    //读取宽度和高度
    inFile.read((char*)&m_uiWidth, sizeof(m_uiWidth));
    inFile.read((char*)(&m_uiHeight), sizeof(m_uiHeight));
    //计算每行字节数
    m_uiBytePerLine = m_uiWidth * 3;
    if (m_uiBytePerLine % 4 != 0) {
        m_uiBytePerLine += (4 - m_uiBytePerLine % 4);
    }
    //重新开辟数据区，调整大小
    delete[] m_ucData;
    m_ucData = new unsigned char[m_uiBytePerLine * m_uiHeight];
    //读入像素数据
    inFile.read((char*)m_ucData, m_uiBytePerLine * m_uiHeight);
    //关闭文件
    inFile.close();
}

/*************************************************************************
【函数名称】Image::SaveToFile
【函数功能】保存到本类自定义格式文件
【参数】入口参数，文件名字符串
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
    2020-05-10 由范静涛做了命名规范化修改
    2020-05-15 由范静涛增加注释
*************************************************************************/
void Image::SaveToFile(const string& FileName) const {
    //打开文件
    ofstream outFile(FileName.c_str(), ios::out | ios::binary);
    //写入宽度和高度
    outFile.write((char*)&m_uiWidth, sizeof(m_uiWidth));
    outFile.write((char*)&m_uiHeight, sizeof(m_uiHeight));
    //写入数据区数据
    outFile.write((char*)m_ucData, m_uiBytePerLine * m_uiHeight);
    //关闭文件
    outFile.close();
}

/*************************************************************************
【函数名称】Image::SaveToJpgFile
【函数功能】保存到jpg格式文件
【参数】入口参数，文件名字符串
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】
    2020-05-15 由范静涛增加注释
【特别提示】此函数稍作修改就可以保存为bmp格式图像文件
*************************************************************************/
void Image::SaveToJpgFile(const string& FileName) const{
    //设定图像质量（100最高），此值越小图像文件尺寸越小
    int quality = 50;
    //用Image对象数据区创建GDI+的bmp图像对象
    Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(Width, Height, BytesPerLine, PixelFormat24bppRGB, m_ucData);
    //jpg编码参数设定
    Gdiplus::EncoderParameters  encoderParameters;
    encoderParameters.Count  =  1;
    encoderParameters.Parameter[0].Guid  =  Gdiplus::EncoderQuality;
    encoderParameters.Parameter[0].Type  =  Gdiplus::EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues  =  1;
    encoderParameters.Parameter[0].Value  =  &quality;
    //获取jpg编码类ID
    CLSID encoderClsid;
    GetJpgEncoderClassID(&encoderClsid);
    //转换文件名到wchar，因为GDI+需要wchar的字符串
    //基本套路，可以直接拿来用，太细节不用理解
    std::string TempStr = FileName;
    int Length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)TempStr.c_str(), -1, NULL,0);
    wchar_t* pWStr = new wchar_t[Length + 1];
    memset(pWStr, 0, Length * 2 + 2);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)TempStr.c_str(), -1, (LPWSTR)pWStr, Length);
    wstring wsFileName = pWStr;
    delete[] pWStr;
    //GDI+的bmp对象使用jpg编码器和参数板寸文件
    pBitmap->Save(wsFileName.c_str(),  &encoderClsid,  &encoderParameters);
    //释放GDI+的bmp对象
    delete pBitmap;
}

/*************************************************************************
【函数名称】Image::SaveToJpgData
【函数功能】保存到jpg
【参数】均为出口参数，分别表示指向jpg内存数据区的指针的指针，指向jpg数据大小的指针
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】
    2020-05-15 由范静涛增加注释
【特别提示】
    pBufer指向的内存空间，必须有此函数调用者手工释放，如SaveToJpgData(&a,&b),则需要delete[] a;
    此函数可与异或操作符连用，获得高质量的网络图像传输
*************************************************************************/
void Image::SaveToJpgData(char** ppBuffer, unsigned int* puLength) const{
    //设定图像质量（100最高），此值越小图像数据尺寸越小
    //此参数调低是为了方便传输，但如果和异或函数结合，求连续两幅图像的差异，此参数可调高，因为差异图像大面积为黑色仍然能获得小数据量
    int quality = 20;
    //用Image对象数据区创建GDI+的bmp图像对象
    Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(Width, Height, BytesPerLine, PixelFormat24bppRGB, m_ucData);
    //jpg编码参数设定
    Gdiplus::EncoderParameters  encoderParameters;
    encoderParameters.Count  =  1;
    encoderParameters.Parameter[0].Guid  =  Gdiplus::EncoderQuality;
    encoderParameters.Parameter[0].Type  =  Gdiplus::EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues  =  1;
    encoderParameters.Parameter[0].Value  =  &quality;
    encoderParameters.Parameter[0].Value  =  &quality;
    //获取jpg编码类ID
    CLSID encoderClsid;
    GetJpgEncoderClassID(&encoderClsid);
    //将bmp对象用jpg编码器和参数保存到流
    IStream* pStream = NULL;
    CreateStreamOnHGlobal( NULL, TRUE, &pStream );
    pBitmap->Save(pStream,  &encoderClsid,  &encoderParameters);
    //释放bmp对象
    delete pBitmap;
    //读流的内存数据到自己开辟的数据区
    HGLOBAL hStreamMem;
    GetHGlobalFromStream(pStream, &hStreamMem);
    *puLength = GlobalSize(hStreamMem);
    BYTE* pStreamData=(BYTE *)GlobalLock(hStreamMem);
    *ppBuffer = new char[*puLength];
    CopyMemory(*ppBuffer, pStreamData, *puLength);
    GlobalUnlock(hStreamMem);
    //释放流和系统内存
    pStream->Release();
    GlobalFree(hStreamMem);
}

/*************************************************************************
【函数名称】Image::GetJpgEncoderClassID
【函数功能】获取jpg编码器类ID
【参数】出口参数，类编码器ID指针
【返回值】-1为找不到jpg编码器，其他值为编码器ID在系统所有编码器中的索引值（第几个编码器）
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-10
【更改记录】
    2020-05-15 由范静涛增加注释
*************************************************************************/
int Image::GetJpgEncoderClassID(CLSID* pClassID) const {
    //此函数太过细节，不需掌握和理解
    unsigned int EncoderCount = 0;
    unsigned int EncoderArraySize = 0;

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&EncoderCount, &EncoderArraySize);
    if(EncoderArraySize == 0)
        return -1;

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(EncoderArraySize));
    if(pImageCodecInfo == NULL)
        return -1;

    Gdiplus::GetImageEncoders(EncoderCount, EncoderArraySize, pImageCodecInfo);

    for(unsigned int j = 0; j < EncoderCount; ++j)
    {
        if(std::wcscmp(pImageCodecInfo[j].MimeType, L"image/jpeg") == 0 )
        {
            *pClassID = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

/*************************************************************************
【函数名称】CapScreenTo
【函数功能】捕获屏幕图像
【参数】出口参数，存储屏幕截图的Image对象的引用
【返回值】无
【开发者及日期】范静涛(fanjingtao@tsinghua.edu.cn) 2020-5-3
【更改记录】
*************************************************************************/
void CapScreenTo(Image& img) {
    // 想象以下，你去银行取钱，银行不允许你自己进入金库的
    // 只能是柜员进去取，再转交给你

    // 获得一个代表显示器的标识符DisplayDC
    // 金库
    HDC DisplayDC = CreateDC(L"DISPLAY", 0, 0, 0);
    // 获得显示器水平像素数——多宽
    // 金库里一捆钱有多少张
    unsigned int Width = GetDeviceCaps(DisplayDC, HORZRES);
    // 获得显示器垂直像素数——多高
    // 金库里一共有多少捆钱
    unsigned int Height = GetDeviceCaps(DisplayDC, VERTRES);

    // 物理显示器是不允许程序直接读取上面的数据
    // 所以我们申请在自己程序里获得一个代表显示器的标识符MemDispDC
    // 柜员，可以进入金库的柜员
    HDC MemDispDC = CreateCompatibleDC(DisplayDC);
    // 在自己内存里，根据显示器宽和高，创建一个可存放显示内容的数据存储区hBitMap
    // 这里只是开辟了存储区，没有给存储区传值
    // 一个钱袋子，能装下金库全部钱的钱袋子
    HBITMAP hBitMap = CreateCompatibleBitmap(DisplayDC, Width, Height);
    // 将标识符MemDispDC和存储区hBitMap关联在一起
    // 从此以后，往标识符MemDispDC上传显示内容，就实际存储在hBitMap存储区里
    // 柜员拿到了这个钱袋子，柜员拿的钱都要放在这个袋子里
    SelectObject(MemDispDC, hBitMap);
    // 把标识符DisplayDC上显示的内容（从左上角到右下角，也就是全部内容）
    // 拷贝到标识符 MemDispDC代表的内容里，也就是放在hBitmap里
    // 柜员拿走把金库里所有的钱都复印了一遍，装在袋子里
    BitBlt(MemDispDC, 0, 0, Width, Height, DisplayDC, 0, 0, SRCCOPY);

    // 取钱的人，带了一个纸箱，把纸箱信息写在bmiHeader上
    BITMAPINFOHEADER bmiHeader;
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = Width;
    bmiHeader.biHeight = Height;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24; // RGB三色，3字节，共24个二进制位
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;

    // 取钱的人有个小怪癖， 每捆钱必须是4的整数倍张，不是的话，后面补白纸
    unsigned int BytesPerLine = bmiHeader.biBitCount * Width / 8;
    if (BytesPerLine % 4 != 0) {
        BytesPerLine += (4 - BytesPerLine % 4);
    }

    // 申请内存保存位图数据
    // 准备好了收钱的纸箱
    unsigned char* Data = new unsigned char[BytesPerLine * Height];
    // 获取位图数据
    // 柜员按客户要求，把袋子里的钱放到纸箱里
    GetDIBits(MemDispDC, (HBITMAP)hBitMap, 0, Height, Data,
              (LPBITMAPINFO)&bmiHeader, DIB_RGB_COLORS);
    // 在这个过程中，柜员把袋子上面的钱，放到了箱子下面
    // 所以，我们拿到的图像数据，屏幕最上面的一行，反倒在数据的最后一行

    img.m_uiWidth = Width;
    img.m_uiHeight = Height;
    img.m_uiBytePerLine = BytesPerLine;
    delete[] img.m_ucData;
    img.m_ucData = new unsigned char[img.m_uiBytePerLine * img.m_uiHeight];

    unsigned int Real_y;

    for (unsigned int y = 0; y < img.m_uiHeight; y++) {
        Real_y = img.m_uiHeight - y - 1;
        memcpy(img.m_ucData + img.m_uiBytePerLine * y, Data + BytesPerLine * Real_y, img.m_uiBytePerLine);
    }

    // 钱袋子消失了
    DeleteObject(hBitMap);
    // 柜员消失了
    DeleteObject(MemDispDC);
    // 金库消失了
    DeleteObject(DisplayDC);
    // 钱也消失了
    delete[]Data;
}


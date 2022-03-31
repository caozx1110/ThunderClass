/*************************************************************************
���ļ�����Image.cpp
������ģ���Ŀ�ġ�ͼ����Ķ���
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
����Ȩ��Ϣ��
    1����Ļ���������Դ�ڶ�CSDNΪ����"_��̶��Ӱ"2ƪԭ������Դ����޸ĺͷ�װ
    ԭ��Ȩ����������Ϊ����ԭ�����£���ѭ CC 4.0 BY-SA ��ȨЭ�飬ת���븽��ԭ�ĳ������Ӻͱ�������
    ԭ�����ӣ�https://blog.csdn.net/Reversess/article/details/27402893
    �����߷�����(fanjingtao@tsinghua.edu.cn)�����������޸Ĳ��ֵ��κΰ�Ȩ�����κ�ʹ���߿������κ��޸ġ������κ�Ŀ��
    CC BY-SA 4.0 ��һ�����Э�飬����������������ҪȨ����
        A. �����Ȩ��������ͨ���κ�ý����κ���ʽ���ơ�������Ʒ��
        B. �ı��Ȩ����������μӹ���ת���ͻ�����Ʒ���д�������������;����������ҵӦ�á�
    2.��������Ϊ������(fanjingtao@tsinghua.edu.cn)ԭ�������������κΰ�Ȩ�����κ�ʹ���߿������κ��޸ġ������κ�Ŀ��
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸ĺͽṹ�����Ż�������˴洢Ϊjpg�͵���jpg���ݵĹ���
    2020-05-15 �ɷ���������ע��
*************************************************************************/

#include <fstream>
#include <windows.h>
#include <wingdi.h>
#include <cstring>
#include "Image.h"

//ʹ��vs��ȡ������ע��
//#pragma comment(lib, "gdiplus")

//Image��˽�о�̬����������ʾ��ǰһ���ж��ٸ�Image��������GDI+��ʼ�����ͷ�
unsigned int Image::m_uImageCount = 0;

/*************************************************************************
���������ơ�Image::Image
���������ܡ����캯��������ָ����ߵ�ȫ��ɫͼ��
��������ȫ��Ϊ��ڲ���������Ϊ����Ϊ��λ��ͼ���Ⱥ͸߶ȣ�Ĭ�ϲ���ֵ��Ϊ100
������ֵ�����캯�������з���ֵ
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
Image::Image(unsigned int W, unsigned int H) : Width(m_uiWidth), Height(m_uiHeight), BytesPerLine(m_uiBytePerLine) {
    //������Ŀ�Ȼ�߶ȣ��׳��쳣
    if (W == 0 || H == 0) {
        throw "Wrong Size";
    }
    m_uiWidth = W;
    m_uiHeight = H;
    //windowҪ�����ڴ���һ��ͼ�����ռ��4�ֽ��������ռ�
    m_uiBytePerLine = m_uiWidth * 3;
    if (m_uiBytePerLine % 4 != 0) {
        m_uiBytePerLine += (4 - m_uiBytePerLine % 4);
    }
    //����������������Ϊ��ɫ��ȫΪ0��
    m_ucData = new unsigned char[m_uiBytePerLine * m_uiHeight];
    memset(m_ucData, 0, m_uiBytePerLine * m_uiHeight);
    //������һ��ͼ��ʱ����ʼ��GDI+
    if (m_uImageCount == 0) {
        GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
    }
    //����������
    m_uImageCount++;
}

/*************************************************************************
���������ơ�Image::~Image
���������ܡ���������
����������
������ֵ���������������з���ֵ
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
Image::~Image() {
    //�ͷ�������
    delete[] m_ucData;
    //�������Լ�
    m_uImageCount--;
    //���һ��ͼ������������ر�GDI+
    if (m_uImageCount == 0) {
        Gdiplus::GdiplusShutdown( m_gdiplusToken);
    }
}

/*************************************************************************
���������ơ�Image::Image(const Image& anImage)
���������ܡ���������
����������ڲ���anImage����ʾԴͼ�����
������ֵ�����캯�������з���ֵ
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
Image::Image(const Image& anImage) : Width(m_uiWidth), Height(m_uiHeight), BytesPerLine(m_uiBytePerLine) {
    m_uiWidth = anImage.m_uiWidth;
    m_uiHeight = anImage.m_uiHeight;
    m_uiBytePerLine = anImage.m_uiBytePerLine;
    m_ucData = new unsigned char[m_uiBytePerLine * m_uiHeight];
    memcpy(m_ucData, anImage.m_ucData, m_uiBytePerLine * m_uiHeight);
    //�������Լ�
    m_uImageCount++;
}

/*************************************************************************
���������ơ�Image::opertor=(const Image& anImage)
���������ܡ���ֵ���������
����������ڲ���anImage����ʾԴͼ�����
������ֵ������ֵ���������
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
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
���������ơ�Image::opertor^(const Image& anImage)
���������ܡ�������������
����������ڲ���anImage����ʾ�������ĵڶ���ͼ�����
������ֵ���������������ͼ�����
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-10 ȡ���˺���ע��
�����ļ�¼��
    2020-05-15 �ɷ���������ע��
*************************************************************************/
Image Image::operator^ (const Image& anImage) const {
    //����ͼ��ߴ粻һ�£��޷�����׳��쳣
    if (m_uiWidth  != anImage.m_uiWidth || m_uiHeight != anImage.m_uiHeight) {
        throw "Different size";
    }
    //�Ե�ǰ����ΪԴ������ͼ�����
    Image Ret(*this);
    //��ͼ����������anImage���������ֽ���򣬽��������ͼ��������
    for (unsigned int idx = 0 ; idx < m_uiBytePerLine * m_uiHeight; idx++) {
        Ret.m_ucData[idx] ^= anImage.m_ucData[idx];
    }
    //�����������ͼ�����
    return Ret;
}

/*************************************************************************
���������ơ�Image::ScanLine
���������ܡ�����ָ���е�������ָ��
����������ڲ�����ʾ�кţ�������Ϊ0��
������ֵ��ָ���е�������ָ��
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
unsigned char* Image::ScanLine(unsigned int y){
    //����Χ�кţ��׳��쳣
    if (y >= Height) {
        throw "Out of range";
    }
    return m_ucData + m_uiBytePerLine * y;
}

/*************************************************************************
���������ơ�Image::GetPixel
���������ܡ�����ָ��λ�õ����ض���
����������ڲ���������Ϊx�����y����
������ֵ��ָ��λ�õ����ض���
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
Pixel Image::GetPixel(unsigned int x, unsigned int y) const {
    //����ֵ����Χ���׳��쳣
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
���������ơ�Image::SetPixel
���������ܡ��趨ָ��λ�õ����ض���
����������ڲ���������Ϊx���ꡢy��������ض���
������ֵ����
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
void Image::SetPixel(unsigned int x, unsigned int y, const Pixel& aPixel) {
    //����ֵ����Χ���׳��쳣
    if (x >= Width || y >= Height) {
        throw "Out of range";
    }
    m_ucData[m_uiBytePerLine * y + 3 * x + 2] = aPixel.R;
    m_ucData[m_uiBytePerLine * y + 3 * x + 1] = aPixel.G;
    m_ucData[m_uiBytePerLine * y + 3 * x + 0] = aPixel.B;
}

/*************************************************************************
���������ơ�Image::LoadFromFile
���������ܡ���ȡ�����Զ����ʽ�ļ���ͼ�����
����������ڲ������ļ����ַ���
������ֵ����
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
void Image::LoadFromFile(const string& FileName) {
    //���ļ�
    ifstream inFile(FileName.c_str(), ios::in | ios::binary);
    if (!inFile) {
        throw "Cann't open file";
    }

    //��ȡ��Ⱥ͸߶�
    inFile.read((char*)&m_uiWidth, sizeof(m_uiWidth));
    inFile.read((char*)(&m_uiHeight), sizeof(m_uiHeight));
    //����ÿ���ֽ���
    m_uiBytePerLine = m_uiWidth * 3;
    if (m_uiBytePerLine % 4 != 0) {
        m_uiBytePerLine += (4 - m_uiBytePerLine % 4);
    }
    //���¿�����������������С
    delete[] m_ucData;
    m_ucData = new unsigned char[m_uiBytePerLine * m_uiHeight];
    //������������
    inFile.read((char*)m_ucData, m_uiBytePerLine * m_uiHeight);
    //�ر��ļ�
    inFile.close();
}

/*************************************************************************
���������ơ�Image::SaveToFile
���������ܡ����浽�����Զ����ʽ�ļ�
����������ڲ������ļ����ַ���
������ֵ����
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸�
    2020-05-15 �ɷ���������ע��
*************************************************************************/
void Image::SaveToFile(const string& FileName) const {
    //���ļ�
    ofstream outFile(FileName.c_str(), ios::out | ios::binary);
    //д���Ⱥ͸߶�
    outFile.write((char*)&m_uiWidth, sizeof(m_uiWidth));
    outFile.write((char*)&m_uiHeight, sizeof(m_uiHeight));
    //д������������
    outFile.write((char*)m_ucData, m_uiBytePerLine * m_uiHeight);
    //�ر��ļ�
    outFile.close();
}

/*************************************************************************
���������ơ�Image::SaveToJpgFile
���������ܡ����浽jpg��ʽ�ļ�
����������ڲ������ļ����ַ���
������ֵ����
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-10
�����ļ�¼��
    2020-05-15 �ɷ���������ע��
���ر���ʾ���˺��������޸ľͿ��Ա���Ϊbmp��ʽͼ���ļ�
*************************************************************************/
void Image::SaveToJpgFile(const string& FileName) const{
    //�趨ͼ��������100��ߣ�����ֵԽСͼ���ļ��ߴ�ԽС
    int quality = 50;
    //��Image��������������GDI+��bmpͼ�����
    Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(Width, Height, BytesPerLine, PixelFormat24bppRGB, m_ucData);
    //jpg��������趨
    Gdiplus::EncoderParameters  encoderParameters;
    encoderParameters.Count  =  1;
    encoderParameters.Parameter[0].Guid  =  Gdiplus::EncoderQuality;
    encoderParameters.Parameter[0].Type  =  Gdiplus::EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues  =  1;
    encoderParameters.Parameter[0].Value  =  &quality;
    //��ȡjpg������ID
    CLSID encoderClsid;
    GetJpgEncoderClassID(&encoderClsid);
    //ת���ļ�����wchar����ΪGDI+��Ҫwchar���ַ���
    //������·������ֱ�������ã�̫ϸ�ڲ������
    std::string TempStr = FileName;
    int Length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)TempStr.c_str(), -1, NULL,0);
    wchar_t* pWStr = new wchar_t[Length + 1];
    memset(pWStr, 0, Length * 2 + 2);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)TempStr.c_str(), -1, (LPWSTR)pWStr, Length);
    wstring wsFileName = pWStr;
    delete[] pWStr;
    //GDI+��bmp����ʹ��jpg�������Ͳ�������ļ�
    pBitmap->Save(wsFileName.c_str(),  &encoderClsid,  &encoderParameters);
    //�ͷ�GDI+��bmp����
    delete pBitmap;
}

/*************************************************************************
���������ơ�Image::SaveToJpgData
���������ܡ����浽jpg
����������Ϊ���ڲ������ֱ��ʾָ��jpg�ڴ���������ָ���ָ�룬ָ��jpg���ݴ�С��ָ��
������ֵ����
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-10
�����ļ�¼��
    2020-05-15 �ɷ���������ע��
���ر���ʾ��
    pBuferָ����ڴ�ռ䣬�����д˺����������ֹ��ͷţ���SaveToJpgData(&a,&b),����Ҫdelete[] a;
    �˺������������������ã���ø�����������ͼ����
*************************************************************************/
void Image::SaveToJpgData(char** ppBuffer, unsigned int* puLength) const{
    //�趨ͼ��������100��ߣ�����ֵԽСͼ�����ݳߴ�ԽС
    //�˲���������Ϊ�˷��㴫�䣬��������������ϣ�����������ͼ��Ĳ��죬�˲����ɵ��ߣ���Ϊ����ͼ������Ϊ��ɫ��Ȼ�ܻ��С������
    int quality = 20;
    //��Image��������������GDI+��bmpͼ�����
    Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(Width, Height, BytesPerLine, PixelFormat24bppRGB, m_ucData);
    //jpg��������趨
    Gdiplus::EncoderParameters  encoderParameters;
    encoderParameters.Count  =  1;
    encoderParameters.Parameter[0].Guid  =  Gdiplus::EncoderQuality;
    encoderParameters.Parameter[0].Type  =  Gdiplus::EncoderParameterValueTypeLong;
    encoderParameters.Parameter[0].NumberOfValues  =  1;
    encoderParameters.Parameter[0].Value  =  &quality;
    encoderParameters.Parameter[0].Value  =  &quality;
    //��ȡjpg������ID
    CLSID encoderClsid;
    GetJpgEncoderClassID(&encoderClsid);
    //��bmp������jpg�������Ͳ������浽��
    IStream* pStream = NULL;
    CreateStreamOnHGlobal( NULL, TRUE, &pStream );
    pBitmap->Save(pStream,  &encoderClsid,  &encoderParameters);
    //�ͷ�bmp����
    delete pBitmap;
    //�������ڴ����ݵ��Լ����ٵ�������
    HGLOBAL hStreamMem;
    GetHGlobalFromStream(pStream, &hStreamMem);
    *puLength = GlobalSize(hStreamMem);
    BYTE* pStreamData=(BYTE *)GlobalLock(hStreamMem);
    *ppBuffer = new char[*puLength];
    CopyMemory(*ppBuffer, pStreamData, *puLength);
    GlobalUnlock(hStreamMem);
    //�ͷ�����ϵͳ�ڴ�
    pStream->Release();
    GlobalFree(hStreamMem);
}

/*************************************************************************
���������ơ�Image::GetJpgEncoderClassID
���������ܡ���ȡjpg��������ID
�����������ڲ������������IDָ��
������ֵ��-1Ϊ�Ҳ���jpg������������ֵΪ������ID��ϵͳ���б������е�����ֵ���ڼ�����������
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-10
�����ļ�¼��
    2020-05-15 �ɷ���������ע��
*************************************************************************/
int Image::GetJpgEncoderClassID(CLSID* pClassID) const {
    //�˺���̫��ϸ�ڣ��������պ����
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
���������ơ�CapScreenTo
���������ܡ�������Ļͼ��
�����������ڲ������洢��Ļ��ͼ��Image���������
������ֵ����
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
*************************************************************************/
void CapScreenTo(Image& img) {
    // �������£���ȥ����ȡǮ�����в��������Լ��������
    // ֻ���ǹ�Ա��ȥȡ����ת������

    // ���һ��������ʾ���ı�ʶ��DisplayDC
    // ���
    HDC DisplayDC = CreateDC(L"DISPLAY", 0, 0, 0);
    // �����ʾ��ˮƽ�������������
    // �����һ��Ǯ�ж�����
    unsigned int Width = GetDeviceCaps(DisplayDC, HORZRES);
    // �����ʾ����ֱ�������������
    // �����һ���ж�����Ǯ
    unsigned int Height = GetDeviceCaps(DisplayDC, VERTRES);

    // ������ʾ���ǲ��������ֱ�Ӷ�ȡ���������
    // ���������������Լ���������һ��������ʾ���ı�ʶ��MemDispDC
    // ��Ա�����Խ�����Ĺ�Ա
    HDC MemDispDC = CreateCompatibleDC(DisplayDC);
    // ���Լ��ڴ��������ʾ����͸ߣ�����һ���ɴ����ʾ���ݵ����ݴ洢��hBitMap
    // ����ֻ�ǿ����˴洢����û�и��洢����ֵ
    // һ��Ǯ���ӣ���װ�½��ȫ��Ǯ��Ǯ����
    HBITMAP hBitMap = CreateCompatibleBitmap(DisplayDC, Width, Height);
    // ����ʶ��MemDispDC�ʹ洢��hBitMap������һ��
    // �Ӵ��Ժ�����ʶ��MemDispDC�ϴ���ʾ���ݣ���ʵ�ʴ洢��hBitMap�洢����
    // ��Ա�õ������Ǯ���ӣ���Ա�õ�Ǯ��Ҫ�������������
    SelectObject(MemDispDC, hBitMap);
    // �ѱ�ʶ��DisplayDC����ʾ�����ݣ������Ͻǵ����½ǣ�Ҳ����ȫ�����ݣ�
    // ��������ʶ�� MemDispDC����������Ҳ���Ƿ���hBitmap��
    // ��Ա���߰ѽ�������е�Ǯ����ӡ��һ�飬װ�ڴ�����
    BitBlt(MemDispDC, 0, 0, Width, Height, DisplayDC, 0, 0, SRCCOPY);

    // ȡǮ���ˣ�����һ��ֽ�䣬��ֽ����Ϣд��bmiHeader��
    BITMAPINFOHEADER bmiHeader;
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = Width;
    bmiHeader.biHeight = Height;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24; // RGB��ɫ��3�ֽڣ���24��������λ
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;

    // ȡǮ�����и�С��񱣬 ÿ��Ǯ������4���������ţ����ǵĻ������油��ֽ
    unsigned int BytesPerLine = bmiHeader.biBitCount * Width / 8;
    if (BytesPerLine % 4 != 0) {
        BytesPerLine += (4 - BytesPerLine % 4);
    }

    // �����ڴ汣��λͼ����
    // ׼��������Ǯ��ֽ��
    unsigned char* Data = new unsigned char[BytesPerLine * Height];
    // ��ȡλͼ����
    // ��Ա���ͻ�Ҫ�󣬰Ѵ������Ǯ�ŵ�ֽ����
    GetDIBits(MemDispDC, (HBITMAP)hBitMap, 0, Height, Data,
              (LPBITMAPINFO)&bmiHeader, DIB_RGB_COLORS);
    // ����������У���Ա�Ѵ��������Ǯ���ŵ�����������
    // ���ԣ������õ���ͼ�����ݣ���Ļ�������һ�У����������ݵ����һ��

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

    // Ǯ������ʧ��
    DeleteObject(hBitMap);
    // ��Ա��ʧ��
    DeleteObject(MemDispDC);
    // �����ʧ��
    DeleteObject(DisplayDC);
    // ǮҲ��ʧ��
    delete[]Data;
}


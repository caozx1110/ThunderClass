/*************************************************************************
���ļ�����Image.h
������ģ���Ŀ�ġ�ͼ���������
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
    2020-05-21 ��������ͬѧ���ִ�ͷ�ļ������������õ����⣬ԭ������Ҫǰ������ͷ�ļ����Ѹ���
*************************************************************************/
#ifndef ImageH
#define ImageH

#include <vector>
#include <string>
//���������ֱ���include��������ͷ�ļ�
#include <windows.h>
#include <wingdi.h>
#include <gdiplus.h> //gdiplus��windows����ϵͳ�Դ����ܼ���֮һ������ͼ������ȣ���jpg���ܵ���Դ
#include <comdef.h>  //��gidplus���ʹ��
#include "Pixel.h"
using namespace std;

/*************************************************************************
��������Image
�����ܡ�����ͼ�����
���ӿ�˵����
    ���캯��Image����ָ����߻�Ĭ��100���ء�100���ص�ȫ��ɫ
    ��������~Image
    �������캯��Image(const Image& anImage);
    ��ֵ���������Image& operator= (const Image& anImage);
    ������������Image operator^ (const Image& anImage);
    ����ScanLine����ָ���е�ԭʼ������ָ��
    ����GetPixel����ָ��λ�õ�����ֵ
    ����SetPixel�趨ָ��λ�õ�����ֵ
    ����LoadFromFile��ȡ�����Զ����ͼ���ʽ�ļ�
    ����SaveToFile����ͼ�񵽱����Զ����ͼ���ʽ�ļ�
    ����SaveToJpgFile����ͼ��Jpg��ʽͼ���ļ�
    ����SaveToJpgData����ͼ��Jpgԭʼ���ݵ�ָ���ڴ�����
    ��Ԫ����CapScreenTo(Image& img)ץȡ��ǰ��Ļ��ָ��ͼ�����
    ���������ݳ�ԱWidth��ʾ������Ϊ��λ��ͼ����
    ���������ݳ�ԱHeight��ʾ������Ϊ��λ�ĸ߶�
    ���������ݳ�ԱBytesPerLine��ʾÿ��ͼ��ռ�˶����ֽڣ�4�ֽ���������
�������߼����ڡ�������(fanjingtao@tsinghua.edu.cn) 2020-5-3
�����ļ�¼��
    2020-05-10 �ɷ��������������淶���޸ĺͽṹ�����Ż�������˴洢Ϊjpg�͵���jpg���ݵĹ���
    2020-05-15 �ɷ���������ע��
*************************************************************************/
class Image{
public:
    //���캯��Image����ָ����߻�Ĭ��100���ء�100���ص�ȫ��ɫ
    Image(unsigned int W = 100, unsigned int H = 100);

    //��������~Image
    ~Image();

    //�������캯��Image(const Image& anImage);
    Image(const Image& anImage);

    //��ֵ���������Image& operator= (const Image& anImage);
    Image& operator= (const Image& anImage);

    //������������Image operator^ (const Image& anImage);
    Image operator^ (const Image& anImage) const;

    //����ScanLine����ָ���е�ԭʼ������ָ��
    unsigned char* ScanLine(unsigned int y);

    //����GetPixel����ָ��λ�õ�����ֵ
    Pixel GetPixel(unsigned int x, unsigned int y) const;

    //����SetPixel�趨ָ��λ�õ�����ֵ
    void SetPixel(unsigned int x, unsigned int y, const Pixel& aPixel);

    //����LoadFromFile��ȡ�����Զ����ͼ���ʽ�ļ�
    void LoadFromFile(const string& FileName);

    //����SaveToFile����ͼ�񵽱����Զ����ͼ���ʽ�ļ�
    void SaveToFile(const string& FileName) const;

    //����SaveToJpgFile����ͼ��Jpg��ʽͼ���ļ�
    void SaveToJpgFile(const string& FileName) const;

    //����SaveToJpgData����ͼ��Jpgԭʼ���ݵ�ָ���ڴ�����
    void SaveToJpgData(char** ppBuffer, unsigned int* puLength) const;

    //��Ԫ����CapScreenTo(Image& img)ץȡ��ǰ��Ļ��ָ��ͼ�����
    friend void CapScreenTo(Image& img);

    //���������ݳ�ԱWidth��ʾ������Ϊ��λ��ͼ����
    const unsigned int& Width;

    //���������ݳ�ԱHeight��ʾ������Ϊ��λ�ĸ߶�
    const unsigned int& Height;

    //���������ݳ�ԱBytesPerLine��ʾÿ��ͼ��ռ�˶����ֽڣ�4�ֽ���������
    const unsigned int& BytesPerLine;

private:
    //��ȡJpg������
    int GetJpgEncoderClassID(CLSID* pClassID) const;

    //��̬˽�г�Ա��һ���ж��ٸ�Image�Ķ���
    static unsigned int m_uImageCount;

    //ͼ����
    unsigned int m_uiWidth;

    //ͼ��߶�
    unsigned int m_uiHeight;

    //��һά����ģ���ά�洢�ṹ
    unsigned char* m_ucData;

    //ÿ������ռ�ö����ֽڣ�4�ֽ���������
    unsigned int m_uiBytePerLine;

    //GDI+ ��ʼ���ͽ����õģ��������
    Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;
};
#endif

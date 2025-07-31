#include <windows.h>        // 为了使用 CP_ACP、CP_UTF8 等常量
#include <ctime>
#include <assert.h>         // 为了使用 assert(false); 
#include "stringapiset.h"   // 为了使用 MultiByteToWideChar()等函数
#include "TransferString.hpp" 


namespace tshsb
{

#define hsb_min(a,b) (((a) < (b)) ? (a) : (b))


// 2024年9月12日 
// 为了兼容旧代码，增加一个宏 USE_ASSERT_FUNC，默认值为1，表示使用assert()函数，否则不使用。
// 对于旧代码，可以将 USE_ASSERT_FUNC 定义为 0，这样就不会使用 assert()函数
void MyAssert(bool bPara)
{
#if USE_ASSERT_FUNC
    assert(bPara);
#endif
}


// 随机生成一个GBK编码的中文字符
std::string generateRandomGBKChar() {
    char high = 0xB0 + rand() % (0xF7 - 0xB0 + 1);
    char low = 0xA1 + rand() % (0xFE - 0xA1 + 1);
    return std::string(1, high) + std::string(1, low);
}


// 生成随机长度的字符串，其中包含随机的英文、数字、GBK编码的中文
// 2024年9月12日-该函数用于测试本文件的转换函数，生成随机字符串
std::string generateRandomGBKString(int length/* = 1000*/) 
{
    srand(time(0));   // 设置随机种子
    std::string strTestRand;
    int totalLength = rand() % length; // 生成0到999之间的随机长度

    for (int i = 0; i < totalLength; ++i) {
        int n = rand() % 3;
        if (n == 0) {
            strTestRand += (char)(rand() % 26 + 'a'); // 随机小写字母
        }
        else if (n == 1) {
            strTestRand += (char)(rand() % 10 + '0'); // 随机数字
        }
        else {
            strTestRand += generateRandomGBKChar(); // 随机GBK编码的中文字符
        }
    }

    return strTestRand;
}


// 2023年12月5日
// 将多字节编码的字符串转为宽字节
// szC    (in):      需要转换的ANSI编码（或utf8编码）的字符串
// szW    (out):     转换完的utf16编码的字符串
// szWlen (in/out):  输入为szW在外部申请的数组长度，输出为字符串实际的长度
// nType  (in):      要转换的字符集类型，CP_ACP=从ANSI转到utf16；CP_UTF8=从utf8转到utf16
bool char2wchar(const char* szC, wchar_t* szW, int& szWlen, int nType/* = CP_ACP*/)
{
    bool bRet = false;
    if (szC == nullptr || szW == nullptr) { MyAssert(false); return bRet; }
    if (nType != CP_ACP && nType != CP_UTF8) { MyAssert(false); return bRet; }

    // -1表示“处理到字串尾”。后面两个参数表示“先不要复制，看看需要多少宽字节”  
    int iLength = MultiByteToWideChar(nType, 0, szC, -1, NULL, 0);
    if (iLength == 0 || iLength > szWlen) {   // 检错
        szWlen = iLength;     // 把这个值输出，以便调用者知道实际的字符串长度
        MyAssert(false);
        return bRet;
    }
    wchar_t* wzsTemp = new wchar_t[iLength + 1] { L'\0' };
    do {
        iLength = MultiByteToWideChar(nType, 0, szC, -1, wzsTemp, iLength);
        if (iLength == 0 || iLength > szWlen) {   // 检错
            szWlen = iLength;     // 把这个值输出，以便调用者知道实际的字符串长度
            MyAssert(false);
            break;
        }
        if (iLength > szWlen) { MyAssert(false); break; }
        // 拷贝赋值：
        wcscpy_s(szW, hsb_min(iLength + 1, szWlen), wzsTemp);
        szWlen = iLength + 1;
    } while (false);
    delete[] wzsTemp;
    return true;
}

// 2023年12月5日
// 将宽字节编码的字符串转为多字节
// szW    (in):     需要转换的utf16编码的字符串
// szC    (out):    转换完的ANSI编码（或utf8编码）的字符串
// szClen (in/out): 输入为szC在外部申请的数组长度，输出为字符串实际的长度
// nType  (in):     要转换的字符集类型，CP_ACP=从utf16转到ANSI；CP_UTF8=从utf16转到utf8
bool wchar2char(const wchar_t* szW, char* szC, int& szClen, int nType/* = CP_ACP*/)
{
    bool bRet = false;
    if (szC == nullptr || szW == nullptr) { MyAssert(false); return bRet; }  // 检错  
    if (nType != CP_ACP && nType != CP_UTF8) { MyAssert(false); return bRet; }

    // -1表示“处理到字串尾”，第5、6两参数表示“先不要复制,看看需要多少宽字节”  
    int len = WideCharToMultiByte(nType, 0, szW, -1, NULL, 0, NULL, NULL);
    if (len == 0 || len > szClen) {   // 检错
        szClen = len;     // 把这个值赋给szClen，以便调用者知道实际的字符串长度
        MyAssert(false); 
        return bRet; 
    }   
    char* cszTemp = new char[len + 1] { '\0' };
    do {
        len = WideCharToMultiByte(nType, 0, szW, -1, cszTemp, len, NULL, NULL);
        if (len == 0 || len > szClen) {   // 检错
            szClen = len;     // 把这个值赋给szClen，以便调用者知道实际的字符串长度
            MyAssert(false);
            break;
        }
        // 拷贝赋值：
        strcpy_s(szC, hsb_min(len + 1, szClen), cszTemp);
        szClen = len + 1;
        bRet = true;
    } while (false);
    delete[] cszTemp;
    return bRet;
}

// 2023年12月10日
// 把string类型的字符串转换为wstring类型。返回值：转换完成的wstring字符串
// str  (in): 需要转换的string类型字符串
// type (in): 输入的string的字符集编码方式，CP_ACP=ANSI编码(GBK编码)；CP_UTF8=utf8编码
std::wstring str2wstr(const std::string& str, int type/* = CP_ACP*/)
{
    std::wstring wstr(L"");
    if (type != CP_ACP && type != CP_UTF8) { MyAssert(false); return wstr; }
    const char* pChar = str.c_str();
    int nLen = str.length() + 1;
    wchar_t* pWChar = new wchar_t[nLen] {'\0'};
    do {
        char2wchar(pChar, pWChar, nLen, type);   // 调用转换函数
        wstr = pWChar;
    } while (false);
    delete[] pWChar;
    return wstr;
}

// 2023年12月10日
// 把wstring类型的字符串转换为string类型。返回值：转换完成的string字符串
// wstr (in): 需要转换的wstring类型字符串
// type (in): 要转换为string的字符集编码方式，CP_ACP=ANSI编码(GBK编码)；CP_UTF8=utf8编码
std::string wstr2str(const std::wstring& wstr, int type/* = CP_ACP*/)
{
    std::string str("");
    if (type != CP_ACP && type != CP_UTF8) { MyAssert(false); return str; }
    const wchar_t* pWChar = wstr.c_str();
    int nLen = wstr.length() * 3 + 1;         // 最长也不会超过这么长
    char* pChar = new char[nLen] {'\0'};
    do {
        wchar2char(pWChar, pChar, nLen, type);    // 调用转换函数
        str = pChar;
    } while (false);
    delete[] pChar;
    return str;
}

// 2024年9月6日-GBK转UTF8
std::string GBKtoUTF8(const std::string& strGBK)
{
    std::string strUTF8("");
    const int nLen = strGBK.length() + 1;
    int nLenUTF16 = nLen;
    wchar_t* pWChar = new wchar_t[nLenUTF16] {'\0'};
    int nLenUTF8 = nLen * 2 + 1;    // 最长也不会超过这么长
    char* cUTF8 = new char[nLenUTF8] {'\0'};
    do {
        // 先把GBK转为UTF16：
        char2wchar(strGBK.c_str(), pWChar, nLenUTF16, CP_ACP);
        // 然后把UTF16转为UTF8：
        wchar2char(pWChar, cUTF8, nLenUTF8, CP_UTF8);
        strUTF8 = cUTF8;
    } while (false);
    delete[] pWChar;
    delete[] cUTF8;
    return strUTF8;
}

// 2024年9月6日-UTF8转GBK
std::string UTF8toGBK(const std::string& strUTF8)
{
    std::string strGBK("");
    const int nLen = strUTF8.length() + 1;
    int nLenUTF16 = nLen;
    wchar_t* pWChar = new wchar_t[nLenUTF16] {'\0'};
    int nLenGBK = nLen * 2 + 1;     // 最长也不会超过这么长
    char* cGBK = new char[nLenGBK] {'\0'};
    do {
        // 先把UTF8转为UTF16
        char2wchar(strUTF8.c_str(), pWChar, nLenUTF16, CP_UTF8);
        // 然后把UTF16转为GBK：
        wchar2char(pWChar, cGBK, nLenGBK, CP_ACP);
        strGBK = cGBK;
    } while (false);
    delete[] pWChar;
    delete[] cGBK;
    return strGBK;
}






} // namespace hsb




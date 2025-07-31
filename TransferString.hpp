#pragma once
#include <string>           // 为了使用string类、wcscpy_s()等函数

/*
* 韩盛柏  2023年12月10日
* 这里用来放所有字符串操作相关的函数
* 
*/

#define USE_ASSERT_FUNC 0   // 2024年9月6日-定义为1时，使用assert()函数，否则不使用assert()函数。按需更改

namespace tshsb
{

// 生成随机长度的字符串，其中包含随机的英文、数字、GBK编码的中文
// 2024年9月12日-该函数用于测试本文件的转换函数，生成随机字符串
std::string generateRandomGBKString(int length = 1000);

// 2023年12月5日
// 将多字节编码的字符串转为宽字节
// szC    (in):      需要转换的ANSI编码（或utf8编码）的字符串
// szW    (out):     转换完的utf16编码的字符串
// szWlen (in/out):  输入为szW在外部申请的数组长度，输出为字符串实际的长度
// nType  (in):      要转换的字符集类型，CP_ACP=从ANSI转到utf16；CP_UTF8=从utf8转到utf16
bool char2wchar(const char* szC, wchar_t* szW, int& szWlen, int nType = CP_ACP);

// 2023年12月5日
// 将宽字节编码的字符串转为多字节
// szW    (in):     需要转换的utf16编码的字符串
// szC    (out):    转换完的ANSI编码（或utf8编码）的字符串
// szClen (in/out): 输入为szC在外部申请的数组长度，输出为字符串实际的长度
// nType  (in):     要转换的字符集类型，CP_ACP=从utf16转到ANSI；CP_UTF8=从utf16转到utf8
bool wchar2char(const wchar_t* szW, char* szC, int& szClen, int nType = CP_ACP);

// 2023年12月10日
// 把string类型的字符串转换为wstring类型。返回值：转换完成的wstring字符串
// str  (in): 需要转换的string类型字符串
// type (in): 输入的string的字符集编码方式，CP_ACP=ANSI编码(GBK编码)；CP_UTF8=utf8编码
std::wstring str2wstr(const std::string& str, int type = CP_ACP);

// 2023年12月10日
// 把wstring类型的字符串转换为string类型。返回值：转换完成的string字符串
// wstr (in): 需要转换的wstring类型字符串
// type (in): 要转换为string的字符集编码方式，CP_ACP=ANSI编码(GBK编码)；CP_UTF8=utf8编码
std::string wstr2str(const std::wstring& wstr, int type = CP_ACP);

// 2024年9月6日-GBK转UTF8
std::string GBKtoUTF8(const std::string& strGBK);

// 2024年9月6日-UTF8转GBK
std::string UTF8toGBK(const std::string& strUTF8);






} // namespace tshsb




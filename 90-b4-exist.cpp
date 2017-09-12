/*1551445 童佳燕 计算机科学与技术1班*/
#define _CRT_SECURE_NO_WARNINGS
#include"90-b4.h"
#include"cmd_console_tools.h"

void GBKToUTF8(const char* strGBK, char* utf8)//将编码转换成UTF8模式
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	strcpy(utf8, str);
	strcat(utf8, "\0");
	if (wstr) delete[] wstr;
	if (str) delete[] str;
}
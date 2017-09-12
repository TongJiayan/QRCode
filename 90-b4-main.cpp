/*1551445 童佳燕 计算机科学与技术1班*/
#include<iostream>
#include"90-b4.h"
#include"cmd_console_tools.h"
using namespace std;
int main()
{
	//system("color f0");//输出cmd界面白底黑字    具体颜色可以用dos命令"color/?"得到列表

	char s[100];
	cout << "请输入要输入的内容【不超过100字节】:" << endl;
	cin.getline(s,100,'\n');//为了扫到空格，不能用cin

	QRcode code(s);
	code.creatQRcode();
	return 0;
}
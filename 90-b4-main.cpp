/*1551445 ͯ���� �������ѧ�뼼��1��*/
#include<iostream>
#include"90-b4.h"
#include"cmd_console_tools.h"
using namespace std;
int main()
{
	//system("color f0");//���cmd����׵׺���    ������ɫ������dos����"color/?"�õ��б�

	char s[100];
	cout << "������Ҫ��������ݡ�������100�ֽڡ�:" << endl;
	cin.getline(s,100,'\n');//Ϊ��ɨ���ո񣬲�����cin

	QRcode code(s);
	code.creatQRcode();
	return 0;
}
#pragma once
/*1551445 ͯ���� �������ѧ�뼼��1��*/

/*����ҵ����1-5-L�汾�Ķ�ά�룬ͼ�α߳�Ϊ37����λ
  �������ɵ���Ϣ���������������С��8���ı��������Դﵽ100������*/

#include<Windows.h>
#include<conio.h>
#define CHARCOUNTLENGTH 8//����ָʾ�� 8 λ
#define N 39 //37+2 ��ά����������
#define LOVERFLOW -3 //��̬���벻���ռ�
#define LERROR -2
#define RIGHT -1

typedef int Status;//��ʾ״̬�����ں�������ֵ

struct QRcodeNode
{
	int value;//0,1ֵ
	int isFunctionGraph;//�Ƿ��ǹ���ͼ�Σ�0��1
};

typedef struct Polymon
{
	int coef;//ϵ��
	int expn;//ָ��
	Polymon *next;
}Polymon,*PolyLink;

class QRcode
{
private:
	int version;//�汾
	char *info;//��Ϣ
	char *utf8;
	char *dataStream;//����λ��
	char *errorCodeStream;//������λ��
	char *codeStream;//������λ���������λ����������
	char *formatInfo;//��ʽ��Ϣ
	int maskId;//������ʽ����
	int NumBitOfData;//������������ݱ���Ŀռ�Ĵ�С
	
	QRcodeNode codeArr[N][N];//��¼0 1 ֵ

	void setSingleLocateValue(int I,int J,int singleSize);//������λͼ�ε����Ͻ�����Լ��߳���7����5��
	//int countNumOfWord();//�����ַ�����
	void addBits();//��Ӳ�λ��
	inline int nToAn(int n);
	inline int AnToN(int An);
	void setGLink(PolyLink *GPolyLhead, int *generatorPolyExpn);//�������ɶ���ʽϵ�����齨������ʽ����
	int maskEvaluate1(QRcodeNode tmp[N][N]);
	int maskEvaluate2(QRcodeNode tmp[N][N]);
	int maskEvaluate3(QRcodeNode tmp[N][N]);
	int maskEvaluate4(QRcodeNode tmp[N][N]);
	void initCodeFunctionPart();//��ʼ����������
	void printCode();
	void dataCharToByteArr();//���ݱ���
	void setInfoPolymon(PolyLink *InfoPLhead);//������Ϣ����ʽ����
	void setGeneratorPolymon(PolyLink *GPolyLhead);//�������ɶ���ʽ��������ϵ�����д���Ǧ��Ĵ���
	void errorCode();//����ʽ�������õ�����Ϣ����ʽ���ж���ʽ�������������µĲ���
	void fillCodeArr();//�����ݱ���+������+ʣ��λ�������ά���������
	void chooseMask();//�������ֱ�׼������������ʽ�����мƷ֣�ѡ�������͵�������ʽ
	void setFormatInfo(QRcodeNode tmp[N][N], char *formatInfo);//�ڸ�ʽ��Ϣ���������Ϣ
	void setCodeArrMask();//����chooseMask����ѡ������ʵ����룬��codeArr������������
public:
	QRcode(const char *s);
	~QRcode();
	void creatQRcode();
};

void GBKToUTF8(const char* strGBK, char* utf8);


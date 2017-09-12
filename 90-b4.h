#pragma once
/*1551445 童佳燕 计算机科学与技术1班*/

/*本作业采用1-5-L版本的二维码，图形边长为37个单位
  可以容纳的信息量，如果汉字数量小于8，文本容量可以达到100及以上*/

#include<Windows.h>
#include<conio.h>
#define CHARCOUNTLENGTH 8//计数指示符 8 位
#define N 39 //37+2 二维码四周留白
#define LOVERFLOW -3 //动态申请不到空间
#define LERROR -2
#define RIGHT -1

typedef int Status;//表示状态，用于函数返回值

struct QRcodeNode
{
	int value;//0,1值
	int isFunctionGraph;//是否是功能图形，0，1
};

typedef struct Polymon
{
	int coef;//系数
	int expn;//指数
	Polymon *next;
}Polymon,*PolyLink;

class QRcode
{
private:
	int version;//版本
	char *info;//信息
	char *utf8;
	char *dataStream;//数据位流
	char *errorCodeStream;//纠错码位流
	char *codeStream;//将数据位流与纠错码位流连接起来
	char *formatInfo;//格式信息
	int maskId;//掩码形式代码
	int NumBitOfData;//是用来存放数据编码的空间的大小
	
	QRcodeNode codeArr[N][N];//记录0 1 值

	void setSingleLocateValue(int I,int J,int singleSize);//单个定位图形的左上角左边以及边长（7或者5）
	//int countNumOfWord();//计算字符数量
	void addBits();//添加补位码
	inline int nToAn(int n);
	inline int AnToN(int An);
	void setGLink(PolyLink *GPolyLhead, int *generatorPolyExpn);//根据生成多项式系数数组建立多项式链表
	int maskEvaluate1(QRcodeNode tmp[N][N]);
	int maskEvaluate2(QRcodeNode tmp[N][N]);
	int maskEvaluate3(QRcodeNode tmp[N][N]);
	int maskEvaluate4(QRcodeNode tmp[N][N]);
	void initCodeFunctionPart();//初始化功能区域
	void printCode();
	void dataCharToByteArr();//数据编码
	void setInfoPolymon(PolyLink *InfoPLhead);//建立消息多项式链表
	void setGeneratorPolymon(PolyLink *GPolyLhead);//建立生成多项式链表，其中系数域中存的是α的次数
	void errorCode();//多项式除法，得到对消息多项式进行多项式除法操作后余下的部分
	void fillCodeArr();//将数据编码+纠错码+剩余位补充进二维码的数组中
	void chooseMask();//四项评分标准，八种掩码形式，进行计分，选择分数最低的掩码形式
	void setFormatInfo(QRcodeNode tmp[N][N], char *formatInfo);//在格式信息区域填充信息
	void setCodeArrMask();//根据chooseMask函数选出最合适的掩码，对codeArr进行掩码设置
public:
	QRcode(const char *s);
	~QRcode();
	void creatQRcode();
};

void GBKToUTF8(const char* strGBK, char* utf8);


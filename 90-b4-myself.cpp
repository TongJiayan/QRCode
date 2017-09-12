/*1551445 童佳燕 计算机科学与技术1班*/
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<cstring>
#include"90-b4.h"
#include"cmd_console_tools.h"
using namespace std;

QRcode::QRcode(const char *s)//初始化二维码的大小，以及
{
	int expn[5] = { 7,10,15,20,26 };//纠错码字数
	int numBitOfDataArr[5] = { 19,34,55,80,108 };
	/*根据字节数决定二维码版本*/
	/*理论上需要根据输入信息的汉字数以及总的总结数进行分段~*/
	/*目前是按照最坏情况打算，也就是全都是汉字*/
	if (strlen(s) < 13)
		version = 1;
	else if (strlen(s) < 23)
		version = 2;
	else if (strlen(s) < 36)
		version = 3;
	else if (strlen(s) < 54)
		version = 4;
	else if (strlen(s) < 73)
		version = 5;
	else
	{
		cout << "ERROR:输入的字符超出指定长度" << endl;
		exit(LERROR);
	}//如果字节数大于140就返回ERROR

	NumBitOfData = numBitOfDataArr[version - 1] * 8;//数据部分位流长度

	int size = 17 + 4 * version;//公式

	//元素初始值为value = 0，isFunctionGraph = 0
	for (int i = 0; i<size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			codeArr[i][j].value = 0;
			codeArr[i][j].isFunctionGraph = 0;//是否为功能区域
		}

	/*存放输入的文本信息*/
	int len = strlen(s);
	info = (char *)malloc(sizeof(char)*len);
	strcpy(info, s);

	dataStream = (char *)malloc(sizeof(char)*NumBitOfData);//数据位流长度 根据纠错级别和版本确定
	errorCodeStream = (char *)malloc(sizeof(char)*(expn[version - 1] * 8+1));//纠错码位流
	errorCodeStream[expn[version - 1] * 8] = '\0';
	int codeStreamLen = NumBitOfData + expn[version - 1] * 8;//数据位流+纠错码位流的长度
	if (version != 1)
		codeStreamLen += 7;//添加上剩余码（该式仅针对与1-5级中，2-4剩余码都为7），最好计算一下，增加可移植性&&&&&可以改进
	codeStream = (char *)malloc(sizeof(char)*(codeStreamLen+1));//申请二维码空间

	formatInfo = (char *)malloc(sizeof(char) * 16);//格式信息
	formatInfo[15] = '\0';
}
QRcode::~QRcode()
{
	free(info);
	free(dataStream);
	free(errorCodeStream);
	free(codeStream);
	free(formatInfo);
}
void QRcode:: setSingleLocateValue(int I,int J,int singleSize)//size=7（定位图形）或者size=5（校正图形）
{
	for(int i=0;i<singleSize;i++)
		for (int j = 0; j < singleSize; j++)
		{
			if (!((i == 1 || i == singleSize - 2)&&j!=0&&j!= singleSize -1 || (j == 1 || j == singleSize - 2)&&i!=0&&i!= singleSize -1))
				codeArr[I + i][J + j].value = 1;
			codeArr[I + i][J + j].isFunctionGraph = 1;
		}
}
void QRcode::setFormatInfo(QRcodeNode tmp[N][N],char *formatInfo)
{
	char *p = formatInfo;
	int size = 17 + 4 * version;//公式
	for (int j = 1; j < 8;)
	{
		if (j== 7)
			tmp[9][j+1].value = (*p) - '0';//遇到定位图形要越过
		else
		    tmp[9][j].value = (*p) - '0';
		tmp[size+1-j][9].value= (*p) - '0';
		p++;
		j++;
	}
	tmp[9][9].value= (*p) - '0';
	tmp[9][size-7].value = (*p) - '0';
	p++;
	tmp[8][9].value = (*p) - '0';
	tmp[9][size - 6].value = (*p) - '0';
	p++;
	for (int i = 6; i > 0; i--)
	{
		tmp[i][9].value = (*p) - '0';
		tmp[9][size + 1 - i].value = (*p) - '0';
		p++;
	}
}
void QRcode::initCodeFunctionPart()
{
	int size = 17 + 4 * version;//公式
	/*四周留白区域,由于在构造函数中以初始化所有元素value值为0*/
	for (int i = 0; i < size + 2; i++)
	{
		codeArr[0][i].isFunctionGraph = 1;
		codeArr[i][0].isFunctionGraph = 1;
		codeArr[size +1][i].isFunctionGraph = 1;
		codeArr[i][size + 1].isFunctionGraph = 1;
	}

	/*三个位置探测图形*/
	setSingleLocateValue(1, 1, 7);
	setSingleLocateValue(size-6, 1, 7);
	setSingleLocateValue(1, size-6, 7);
	if (version>1)//校正图形在2级及以上才有
		setSingleLocateValue(size -8, size-8, 5);

	/*分隔符区域*/
	for (int i = 0; i <8; i++)
	{
		codeArr[8][i + 1].isFunctionGraph = 1;
		codeArr[i + 1][8].isFunctionGraph = 1;

		codeArr[8][i + size-6].isFunctionGraph = 1;
		codeArr[i + 1][size-7].isFunctionGraph = 1;

		codeArr[size-7][i + 1].isFunctionGraph = 1;
		codeArr[i + size-6][8].isFunctionGraph = 1;
	}

	/*定位图形*/
	for (int i = 9; i < N - 8; i++)
	{
		codeArr[7][i].isFunctionGraph = 1;
		codeArr[i][7].isFunctionGraph = 1;
		if (i % 2)
		{
			codeArr[i][7].value = 1;
			codeArr[7][i].value = 1;
		}
	}
	/*格式信息区域*/
	for (int i = 0; i <9; i++)
	{
		codeArr[9][i + 1].isFunctionGraph = 1;
		codeArr[i + 1][9].isFunctionGraph = 1;
		codeArr[9][i + size - 7].isFunctionGraph = 1;
		codeArr[i + size -7][9].isFunctionGraph = 1;
	}

	/*小黑点*/
	codeArr[size - 7][9].isFunctionGraph = 1;
	codeArr[size - 7][9].value = 1;
}
void QRcode::printCode()
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	setfontsize(hout, L"点阵字体", 16);
	int size = 17 + 4 * version;//公式
	for (int i = 0; i < size + 2; i++)
	{
		for (int j = 0; j < size + 2; j++)
			if (codeArr[i][j].value)
			{
				setcolor(hout, 0, 0); 
				printf("  ");
			}
			else
			{
				setcolor(hout, 15, 15);
				printf("  ");
			}		
		cout << endl;
	}
}

/*获取输入字符串的字符个数，要解决汉字问题~   //好像不需要了，前期理解认识性错误
int QRcode::countNumOfWord()
{
	int length = 0;
	for (int i = 0; i < (signed)strlen(info);)
	{
		if ((int)info[i] < 0)//GB2312中，汉字字符的第一个字节的十进制数小于零；碰到之后就越过下一个字节
			i += 2;
		else
			i++;
		length++;
	}
	return length;
}*/  


/*编码数据=模式指示符+计数指示符+编码后原串+补位*/
void QRcode::dataCharToByteArr()
{
	void addBits(char *dataStream, int maxNumBit);
	
	/*数据编码后原串*/
	int len = strlen(info);
	char *utf8 = (char *)malloc(sizeof(char)*len * 2);
	if (utf8 == NULL)
		exit(LOVERFLOW);
	GBKToUTF8(info, utf8);//GB->UTF8

	int length = strlen(utf8);
	char charCountIndicator[CHARCOUNTLENGTH + 1];//1-9版本的计数指示符为八位长
	charCountIndicator[CHARCOUNTLENGTH] = '\0';//尾零的重要性,字符数组，在字符串连接等操作时，尾零~~~~~
	for (int i = CHARCOUNTLENGTH - 1; i >= 0; i--)
	{
		charCountIndicator[i] = (char)((length & 0x0001) + '0');//转换成二进制流
		length >>= 1;
	}

	char *binary = (char *)malloc(sizeof(char)*len * 16);//存放数据编码后原串
	if (binary == NULL)
		exit(LOVERFLOW);
	char a;//临时变量，将每个字节存储为short型~
	int bitCount = 8;
	for (int i = 0; utf8[i]; i++)
	{
		a = utf8[i];
		for (int j = 0; j < 8; j++)
		{
			binary[--bitCount] = char((a & 0x0001)+'0');//逆序提取，因此需要逆序存储才可以得到正确顺序
			a >>= 1;
		}
		bitCount += 16;
	}
	binary[bitCount - 8] = '\0';//及其重要，强调在字符数组做字符串操作时尾零的重要性
	free(utf8);

	char modeIndicator[5] = "0100";//二进制模式指示符为0100
	//char modeIndicator[5] = "0001";//改回来，仅仅为了测试8796
//	int length = countNumOfWord();//字符个数，如“us我，,”是5个字符
	

	strcpy(dataStream, modeIndicator);
	strcat(dataStream, charCountIndicator);//与后面多了一个字节的长度。。不知道哪来的~【尾零忘了。。】
	strcat(dataStream, binary);
	QRcode::addBits();
	//cout << "datastream="<<dataStream << endl;
	free(binary);
	/*for (int i = 0; i < NumBitOfData; i++)
	{
		cout << dataStream[i];
		if ((i + 1) % 8 == 0)
			cout << endl;
	}*/
	
}



void QRcode::addBits()
{
	int numZero = 0;//补位个数
	if (NumBitOfData - strlen(dataStream)< 8)
		numZero = NumBitOfData - strlen(dataStream);
	else
	numZero = 8 - ((strlen(dataStream) % 8) ? (strlen(dataStream) % 8) : 8);

	for (int i = 0; i < numZero; i++)
		strcat(dataStream, "0");

	char addString1[] = "11101100";
	char addString2[] = "00010001";
	
	int x = (NumBitOfData - strlen(dataStream)) / 8;
	/*cout << "NumBitOfData=" << NumBitOfData << endl;
	cout << "strlen(dataStream)=" << strlen(dataStream) << endl;
	cout << "x=" << x << endl;
	getchar();*/
	/*两个字符串交替填充*/
	for (int i = 0; i < x; i++)
		if (i % 2)
			strcat(dataStream, addString2);
		else
			strcat(dataStream, addString1);
}

void QRcode::setInfoPolymon(PolyLink *InfoPolyLhead)//建立消息多项式
{
	int expn[5] = { 7,10,15,20,26 };//不同的版本对应的纠错码字数
	int numByteOfData = NumBitOfData / 8;//数据编码的字节数量
	int expnValue = numByteOfData + expn[version - 1];//根据生成多项式，建立的消息多项式的最高次数加一
	PolyLink p =*InfoPolyLhead;

	/*小心，消息多项式的链表除了要建立有系数的项，系数为0的项也需要建立，
	因为在降次的过程中，会与其进行异或运算，最终留下来的，也必定是最后几项*/
	for (int i = 0; i < NumBitOfData+8* expn[version - 1]; i += 8)
	{
		PolyLink s = (PolyLink)malloc(sizeof(Polymon));
		if (s == NULL)
			exit(LOVERFLOW);

		if (i < NumBitOfData)
		{
			int coefValue = 0;
			for (int j = i; j < i + 8; j++)
			{
				coefValue <<= 1;
				coefValue += (dataStream[j] - '0');
			}
			s->coef = coefValue;
		}
		else
			s->coef = 0;//对消息多项式进行升次之后，低次的系数为零
		s->expn = --expnValue;
		p->next = s;
		p = s;
	}
	p->next = NULL;
}


void QRcode::setGeneratorPolymon(PolyLink *GPolyLhead)//建立生成多项式的单项链表(系数为A^n次，该数组存储的是n，依次排列)
{
	/*由于选择的版本较低，生成多项式的系数以α^n的形式中的n进行直接打表，数组存储，后续在存储进链表中*/
	//该处是否需要存储进链表，还需考虑，暂做记号，因为发现后续只用到了这些n&&&&&&&
	if (version == 1)
	{
		int generatorPolyExpn[] = { 0,87,229,146,149,238,102,21 };
		setGLink(GPolyLhead, generatorPolyExpn);
	}
	else if (version == 2)
	{
		int generatorPolyExpn[] = { 0,251,67,46,61,118,70,64,94,32,45 };
		setGLink(GPolyLhead, generatorPolyExpn);
	}
	else if (version == 3)
	{
		int generatorPolyExpn[] = { 0,8,183,61,91,202,37,51,58,58,237,140,124,5,99,105 };
		setGLink(GPolyLhead, generatorPolyExpn);
	}
	else if (version == 4)
	{
		int generatorPolyExpn[] = { 0,17,60,79,50,61,163,26,187,202,180,221,225,83,239,156,164,212,212,188,190 };
		setGLink(GPolyLhead, generatorPolyExpn);
	}
	else if (version == 5)
	{
		int generatorPolyExpn[] = { 0,173,125,158,2,103,182,118,17,145,201,111,28,165,53,161,21,245,142,13,102,48,227,153,145,218,70 };
		setGLink(GPolyLhead, generatorPolyExpn);
	}
	else
		exit(LERROR);
}

void QRcode::setGLink(PolyLink *GPolyLhead,int *generatorPolyExpn)
{
	int expn[5] = { 7,10,15,20,26 };//版本对应的生成多项式次数
	PolyLink p = *GPolyLhead;
	int hExpn = expn[version - 1];//生成多项式的最高项次数
	for (int i = 0; i <= hExpn; i++)
	{
		PolyLink s = (PolyLink)malloc(sizeof(Polymon));
		if (s == NULL)
			exit(LOVERFLOW);
		s->coef = generatorPolyExpn[i];//生成多项式系数域中存的是α的次数
		s->expn = hExpn-i;//次数高的项在前面
		p->next = s;
		p = s;
	}
	p->next = NULL;
}


void QRcode::errorCode()
{
	PolyLink GPolyLhead = (PolyLink)malloc(sizeof(Polymon));//生成多项式单链表
	if (GPolyLhead == NULL)
		exit(LOVERFLOW);
	setGeneratorPolymon(&GPolyLhead);

	PolyLink InfoPLhead = (PolyLink)malloc(sizeof(Polymon));//建立消息多项式链表
	if (InfoPLhead == NULL)
		exit(LOVERFLOW);
	setInfoPolymon(&InfoPLhead);

	/*PolyLink pp = GPolyLhead->next;
	cout << "生成多项式为" << endl;
	for (int i = 0; pp; i++)
	{
		cout << pp->coef << "*X^" << pp->expn << endl;
		pp = pp->next;
	}

	pp = InfoPLhead->next;
	cout << "消息多项式为" << endl;
	while (pp)
	{
		cout << pp->coef << "*X^" << pp->expn << endl;
		pp= pp->next;
	}*/

	PolyLink p = InfoPLhead->next;
	PolyLink q = GPolyLhead->next;
	int infoHExpn = p->expn;
	int GHExpn = q->expn;
	for (int i = 0; i<=infoHExpn - GHExpn ; i++)//一直进行多项式相除，直到剩余项数为 生成多项式的次数
	{
		if (p->coef == 0)
		{
			p = p->next;
			continue;//如果系数已经是0，就直接过
		}
		int addExpn = AnToN(p->coef);//将消息多项式最高项的系数化成α^n的形式，使与生成多项式系数相乘时可以简化成指数相加，避免数据溢出
		q = GPolyLhead->next;
		PolyLink s = p;
		for (int j = 0; j <= GHExpn; j++)
		{
			//里德所罗门算法（多项式相除）的精髓，具体原理资料出处：
		    //http://tiierr.xyz/%E4%BA%8C%E7%BB%B4%E7%A0%81-%E6%9C%80%E7%BB%88%E7%BC%96%E7%A0%81/ 系列博客
			int ceofTmp = q->coef + addExpn;
			if (ceofTmp > 255)
				ceofTmp = ceofTmp % 255;

			s->coef = (s->coef) ^( nToAn(ceofTmp));
			s = s->next;
			q = q->next;
		}
		InfoPLhead->next = p->next;
		free(p);//随时释放消息多项式结点
		p = InfoPLhead->next;
	}
	
	/*p = InfoPLhead->next;
	cout << "余数多项式为" << endl;
	while (p)
	{
		cout << p->coef << "*X^" << p->expn << endl;
		p = p->next;
	}*/
	
	/*将十进制纠错码转换成位流*/
	p = InfoPLhead->next;
	int index = 8;


	while (p)
	{
		int errorCodeOct = p->coef;
		for (int i=0;i<8;i++)
		{
			errorCodeStream[--index] = char((errorCodeOct & 0x01) + '0');
			errorCodeOct >>= 1;
		}
		index += 16;
		p = p->next;
	}
	
	/*释放生成多项式的链表*/
	PolyLink w = GPolyLhead, ww;
	while (w)
	{
		ww = w->next;
		free(w);
		w = ww;
	}
	/*释放消息多项式的链表*/
    w = InfoPLhead;
	while (w)
	{
		ww = w->next;
		free(w);
		w = ww;
	}
	
	/*cout << "纠错码数据流为" << endl;
	int i = 0;
	while (errorCodeStream[i])
	{
		cout << (int)errorCodeStream[i];
		i++;
		if (i % 8 == 0)
			cout << endl;
	}*/
}

inline int QRcode::nToAn(int n)//函数篇幅较短，内置提高运行效率
{
	int An = 1;
	for (int i = 0; i < n; i++)
	{
		An*=2;
		if (An > 255)
			An = An ^ 285;
	}
	return An;
}

inline int QRcode::AnToN(int An)
{
	if (An < 1 || An>255)
		return LERROR;//-2
	for (int i = 1; i < 256; i++)
		if (nToAn(i) == An)
			return i;
	return RIGHT;//-1
}

void QRcode::fillCodeArr()
{
	strcpy(codeStream, dataStream);
	strcat(codeStream, errorCodeStream);
	if (version != 1)//同根据1-5版本的特性直接定制，建议改进，利用公式进行计算
		strcat(codeStream, "0000000");
	char *p = codeStream;
	int size = 17 + 4 * version;
	for (int j = size - 1; j > 0;j-=2 )
	{
		if ((j / 2) % 2 == 0)
			for (int i = size; i > 0; i--)//从下到上
			{
				if (codeArr[i][j + 1].isFunctionGraph == 0)//越过功能区域
				{
					if (*p == '1')//元素value值初始化为0，故*p=‘0’是不需要处理的
						codeArr[i][j + 1].value = 1;
					p++;//只要遇到非功能区，指针都需要后移
				}
				if (codeArr[i][j].isFunctionGraph == 0)
				{
					if (*p == '1')
						codeArr[i][j].value = 1;
					p++;
				}
			}
		else if ((j / 2) % 2 == 1)//从上到下
			for (int i = 1; i <= size; i++)
			{
				if (codeArr[i][j + 1].isFunctionGraph == 0)
				{
					if (*p == '1')
						codeArr[i][j + 1].value = 1;
					p++;
				}
				if (codeArr[i][j].isFunctionGraph == 0)
				{
					if (*p == '1')
						codeArr[i][j].value = 1;
					p++;
				}
			}

	}
	return;
}

int QRcode::maskEvaluate1(QRcodeNode tmp[N][N])
{
	int score = 0;
	int size = 17 + 4 * version;//公式
	for (int i = 1; i < size+1; i++)
	{
		for (int j = 1; j < size-3;)
			for (int k = j + 1; k < size+1; k++)
			{
				if (tmp[i][k].value != tmp[i][j].value)
				{
					if (k - j >= 5)
						score+= (3 + k - j - 5);
					j = k;
					break;
				}
				if (k == size)
				{
					if (k - j >= 4)
						score += (3 + k - j - 4);
					j = k;
					break;
				}
			}
	}
	for (int j = 1; j < size+1; j++)
	{
		for (int i = 1; i< size-3;)
			for (int k = i + 1; k < size+1; k++)
			{
				if (tmp[k][j].value != tmp[i][j].value)
				{
					if (k - i >= 5)
						score += (3 + k - i - 5);
					i = k;
					break;
				}
				if (k == size)
				{
					if (k - i >= 4)
						score += (3 + k - i - 4);
					i = k;
					break;
				}
			}
	}
	//cout << score1 << endl;;
	return score;
}
int QRcode::maskEvaluate2(QRcodeNode tmp[N][N])
{
	int score = 0;
	int size = 17 + 4 * version;//公式
	for (int i = 1; i < size; i++)
	{
		if (i < 8)
		{
			for (int j = 8; j < size-7; j++)
				if (tmp[i][j].value == tmp[i][j + 1].value&&tmp[i][j].value == tmp[i + 1][j].value&&tmp[i][j].value == tmp[i + 1][j + 1].value)
					score += 3;
		}
		else if (i > size-8)
		{
			for (int j = 8; j < size; j++)
				if (tmp[i][j].value == tmp[i][j + 1].value&&tmp[i][j].value == tmp[i + 1][j].value&&tmp[i][j].value == tmp[i + 1][j + 1].value)
					score += 3;
		}
		else
			for (int j = 1; j < size; j++)
				if (tmp[i][j].value == tmp[i][j + 1].value&&tmp[i][j].value == tmp[i + 1][j].value&&tmp[i][j].value == tmp[i + 1][j + 1].value)
					score+= 3;
	}
	score += 36;//位置探测符中固定的36分
	return score;
}
int QRcode::maskEvaluate3(QRcodeNode tmp[N][N])
{
	int score = 0;
	int size = 17 + 4 * version;//公式
	for (int i = 1; i <size+1; i++)
		for (int j = 1; j < size-8; j++)
			if (tmp[i][j].value == 1 && tmp[i][j + 1].value == 0 && tmp[i][j + 2].value == 1
				&& tmp[i][j + 3].value == 1 && tmp[i][j + 4].value == 1 && tmp[i][j + 5].value == 0 &&
				tmp[i][j + 6].value == 1 && tmp[i][j + 7].value == 0 && tmp[i][j + 8].value == 0 &&
				tmp[i][j + 9].value == 0 && tmp[i][j + 10].value == 0 || tmp[i][j].value == 0 && tmp[i][j + 1].value == 0 && tmp[i][j + 2].value == 0
				&& tmp[i][j + 3].value == 0 && tmp[i][j + 4].value == 1 && tmp[i][j + 5].value == 0 &&
				tmp[i][j + 6].value == 1 && tmp[i][j + 7].value == 1 && tmp[i][j + 8].value == 1 &&
				tmp[i][j + 9].value == 0 && tmp[i][j + 10].value == 1)
				score += 40;
	for (int j = 1; j < size+1; j++)
		for (int i = 1; i <size-8; i++)
			if (tmp[i][j].value == 1 && tmp[i + 1][j].value == 0 && tmp[i + 2][j].value == 1
				&& tmp[i + 3][j].value == 1 && tmp[i + 4][j].value == 1 && tmp[i + 5][j].value == 0 &&
				tmp[i + 6][j].value == 1 && tmp[i + 7][j].value == 0 && tmp[i + 8][j].value == 0 &&
				tmp[i + 9][j].value == 0 && tmp[i + 10][j].value == 0 || tmp[i][j].value == 0 && tmp[i + 1][j].value == 0 && tmp[i + 2][j].value == 0
				&& tmp[i + 3][j].value == 0 && tmp[i + 4][j].value == 1 && tmp[i + 5][j].value == 0 &&
				tmp[i + 6][j].value == 1 && tmp[i + 7][j].value == 1 && tmp[i + 8][j].value == 1 &&
				tmp[i + 9][j].value == 0 && tmp[i + 10][j].value == 1)
				score+= 40;
	return score;
}
int QRcode::maskEvaluate4(QRcodeNode tmp[N][N])
{
	int score = 0;
	double dark = 0;
	int size = 17 + 4 * version;//公式
	for (int i = 1; i < size+1; i++)
		for (int j = 0; j < size+1; j++)
			if (tmp[i][j].value)
				dark++;
	int tm = (int)(dark / (size*size) * 100);
	int scoreA = (abs((tm / 5 * 5) - 50)) / 5;
	int scoreB = (abs(((tm / 5 + 1) * 5) - 50)) / 5;
	score = scoreA <= scoreB ? (scoreA * 10) : (scoreB * 10);
	return score;
}

void QRcode::chooseMask()
{
	int size = 17 + 4 * version;//公式
	QRcodeNode tmp[N][N];
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
			tmp[i][j].isFunctionGraph = codeArr[i][j].isFunctionGraph;

	int minScore = 0, score = 0;
	strcpy(formatInfo, "111011111000100");
	/*掩码模式0 对应格式信息*/
	setFormatInfo(tmp, formatInfo);
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 &&(i + j) % 2 == 0)
				tmp[i][j].value = !(codeArr[i][j].value);//~不可用，要用！
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	maskId = 0;
	minScore = score;

	/*掩码模式1*/
	setFormatInfo(tmp, "111001011110011");
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 &&  (i-1) % 2 == 0)
				tmp[i][j].value = !(codeArr[i][j].value);// ^ (i % 2);
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	if (score < minScore)
	{
		maskId = 1;
		minScore = score;
		strcpy(formatInfo, "111001011110011");
	}

	/*掩码模式2*/
	setFormatInfo(tmp, "111110110101010");
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 && (j-1) % 3 == 0)
				tmp[i][j].value = !(codeArr[i][j].value);// (j % 3) ^
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	if (score < minScore)
	{
		maskId = 2;
		minScore = score;
		strcpy(formatInfo, "111110110101010");
	}

	/*掩码模式3*/
	setFormatInfo(tmp, "111100010011101");
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 &&  (i + j-2) % 3 == 0)
				tmp[i][j].value = !(codeArr[i][j].value);//((i + j) % 3)
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	if (score < minScore)
	{
		maskId = 3;
		minScore = score;
		strcpy(formatInfo, "111100010011101");
	}

	/*掩码模式4*/
	setFormatInfo(tmp, "110011000101111");
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			int v = (((i-1) % 2) ? ((i-1) / 2 + 1) : ((i-1) / 2)) + (((j-1) % 3) ? ((j-1) / 3 + 1) : ((j-1) / 3));
			if (tmp[i][j].isFunctionGraph == 0 &&  v % 2 == 0)
				tmp[i][j].value = !(codeArr[i][j].value);// (v % 2)^(codeArr[i][j].value);
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	if (score < minScore)
	{
		maskId = 4;
		minScore = score;
		strcpy(formatInfo, "110011000101111");
	}

	/*掩码模式5*/
	setFormatInfo(tmp, "110001100011000");
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 && (((i-1)*(j-1)) % 3 + ((i-1)*(j-1)) % 2) == 0)
				tmp[i][j].value = !(codeArr[i][j].value);//((i*j) % 3 + (i*j) % 2)^
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	if (score < minScore)
	{
		maskId = 5;
		minScore = score;
		strcpy(formatInfo, "110001100011000");//纠错级别代码+掩码代码+纠错位，经过与101010000010010异或操作后
	}

	/*掩码模式6*/
	setFormatInfo(tmp, "110110001000001");
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 && ((((i-1)*(j-1)) % 3 + ((i-1)*(j-1)) % 2) % 2) == 0)
				tmp[i][j].value = !(codeArr[i][j].value);//(((i*j) % 3 + (i*j) % 2) % 2)^
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	if (score < minScore)
	{
		maskId = 6;
		minScore = score;
		strcpy(formatInfo, "110110001000001");
	}

	/*掩码模式7*/
	setFormatInfo(tmp, "110100101110110");
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 && (((i + j-2) % 3 + (i + j-2) % 2) % 2) == 0)
				tmp[i][j].value = !(codeArr[i][j].value);//(((i + j) % 3 + (i + j) % 2) % 2)^
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	if (score < minScore)
	{
		maskId = 7;
		minScore = score;
		strcpy(formatInfo, "110100101110110");
	}
	
}

void QRcode::setCodeArrMask()
{
	int size = 17 + 4 * version;//公式
	setFormatInfo(codeArr, formatInfo);
	switch (maskId)
	{
	case 0:
		for (int i = 1; i < size + 2; i++)
			for (int j = 1; j < size + 2; j++)
				if (codeArr[i][j].isFunctionGraph == 0 && (i + j) % 2 == 0)
					codeArr[i][j].value = !(codeArr[i][j].value);//((i + j) % 2)^
		break;

	case 1:
		for (int i = 0; i<size + 2; i++)
			for (int j = 0; j < size + 2; j++)
				if (codeArr[i][j].isFunctionGraph == 0 &&(i-1)%2==0)
					codeArr[i][j].value = !(codeArr[i][j].value);//(i % 2)^
		break;

	case 2:
		for (int i = 0; i<size + 2; i++)
			for (int j = 0; j < size + 2; j++)
				if (codeArr[i][j].isFunctionGraph == 0 &&(j-1)%3==0)
					codeArr[i][j].value =!(codeArr[i][j].value);// (j % 3)^
		break;

	case 3:
		for (int i = 0; i<size + 2; i++)
			for (int j = 0; j < size + 2; j++)
				if (codeArr[i][j].isFunctionGraph == 0 && (i+j-2)%3==0)
					codeArr[i][j].value = !(codeArr[i][j].value);//((i + j) % 3)^
		break;
		
	case 4:
		for (int i = 0; i<size + 2; i++)
			for (int j = 0; j < size + 2; j++)
			{
				int v = (((i - 1) % 2) ? ((i - 1) / 2 + 1) : ((i - 1) / 2)) + (((j - 1) % 3) ? ((j - 1) / 3 + 1) : ((j - 1) / 3));
				if (codeArr[i][j].isFunctionGraph == 0 && v%2==0)
					codeArr[i][j].value = !(codeArr[i][j].value);//(v % 2)^
			}
		break;

	case 5:
		for (int i = 0; i<size + 2; i++)
			for (int j = 0; j < size + 2; j++)
				if (codeArr[i][j].isFunctionGraph == 0 && (((i-1)*(j-1)) % 3 + ((i-1)*(j-1)) % 2)==0)
					codeArr[i][j].value =!(codeArr[i][j].value); //((i*j) % 3 + (i*j) % 2) ^
		break;

	case 6:
		for (int i = 0; i<size + 2; i++)
			for (int j = 0; j < size + 2; j++)
				if (codeArr[i][j].isFunctionGraph == 0 &&  ((((i-1)*(j-1)) % 3 + ((i-1)*(j-1)) % 2) % 2)==0)
					codeArr[i][j].value =!(codeArr[i][j].value);//(((i*j) % 3 + (i*j) % 2) % 2)^
		break;

	case 7:
		for (int i = 0; i<size + 2; i++)
			for (int j = 0; j < size + 2; j++)
				if (codeArr[i][j].isFunctionGraph == 0 && (((i + j-2) % 3 + (i + j-2) % 2) % 2)==0)
					codeArr[i][j].value = !(codeArr[i][j].value);//(((i + j) % 3 + (i + j) % 2) % 2)^
		break;
		
	default:break;
	}
}

void QRcode::creatQRcode()
{
	initCodeFunctionPart();
	dataCharToByteArr();
	errorCode();
	fillCodeArr();
	chooseMask();
	setCodeArrMask();
	printCode();
}

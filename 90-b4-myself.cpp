/*1551445 ͯ���� �������ѧ�뼼��1��*/
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<cstring>
#include"90-b4.h"
#include"cmd_console_tools.h"
using namespace std;

QRcode::QRcode(const char *s)//��ʼ����ά��Ĵ�С���Լ�
{
	int expn[5] = { 7,10,15,20,26 };//����������
	int numBitOfDataArr[5] = { 19,34,55,80,108 };
	/*�����ֽ���������ά��汾*/
	/*��������Ҫ����������Ϣ�ĺ������Լ��ܵ��ܽ������зֶ�~*/
	/*Ŀǰ�ǰ����������㣬Ҳ����ȫ���Ǻ���*/
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
		cout << "ERROR:������ַ�����ָ������" << endl;
		exit(LERROR);
	}//����ֽ�������140�ͷ���ERROR

	NumBitOfData = numBitOfDataArr[version - 1] * 8;//���ݲ���λ������

	int size = 17 + 4 * version;//��ʽ

	//Ԫ�س�ʼֵΪvalue = 0��isFunctionGraph = 0
	for (int i = 0; i<size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			codeArr[i][j].value = 0;
			codeArr[i][j].isFunctionGraph = 0;//�Ƿ�Ϊ��������
		}

	/*���������ı���Ϣ*/
	int len = strlen(s);
	info = (char *)malloc(sizeof(char)*len);
	strcpy(info, s);

	dataStream = (char *)malloc(sizeof(char)*NumBitOfData);//����λ������ ���ݾ�����Ͱ汾ȷ��
	errorCodeStream = (char *)malloc(sizeof(char)*(expn[version - 1] * 8+1));//������λ��
	errorCodeStream[expn[version - 1] * 8] = '\0';
	int codeStreamLen = NumBitOfData + expn[version - 1] * 8;//����λ��+������λ���ĳ���
	if (version != 1)
		codeStreamLen += 7;//�����ʣ���루��ʽ�������1-5���У�2-4ʣ���붼Ϊ7������ü���һ�£����ӿ���ֲ��&&&&&���ԸĽ�
	codeStream = (char *)malloc(sizeof(char)*(codeStreamLen+1));//�����ά��ռ�

	formatInfo = (char *)malloc(sizeof(char) * 16);//��ʽ��Ϣ
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
void QRcode:: setSingleLocateValue(int I,int J,int singleSize)//size=7����λͼ�Σ�����size=5��У��ͼ�Σ�
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
	int size = 17 + 4 * version;//��ʽ
	for (int j = 1; j < 8;)
	{
		if (j== 7)
			tmp[9][j+1].value = (*p) - '0';//������λͼ��ҪԽ��
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
	int size = 17 + 4 * version;//��ʽ
	/*������������,�����ڹ��캯�����Գ�ʼ������Ԫ��valueֵΪ0*/
	for (int i = 0; i < size + 2; i++)
	{
		codeArr[0][i].isFunctionGraph = 1;
		codeArr[i][0].isFunctionGraph = 1;
		codeArr[size +1][i].isFunctionGraph = 1;
		codeArr[i][size + 1].isFunctionGraph = 1;
	}

	/*����λ��̽��ͼ��*/
	setSingleLocateValue(1, 1, 7);
	setSingleLocateValue(size-6, 1, 7);
	setSingleLocateValue(1, size-6, 7);
	if (version>1)//У��ͼ����2�������ϲ���
		setSingleLocateValue(size -8, size-8, 5);

	/*�ָ�������*/
	for (int i = 0; i <8; i++)
	{
		codeArr[8][i + 1].isFunctionGraph = 1;
		codeArr[i + 1][8].isFunctionGraph = 1;

		codeArr[8][i + size-6].isFunctionGraph = 1;
		codeArr[i + 1][size-7].isFunctionGraph = 1;

		codeArr[size-7][i + 1].isFunctionGraph = 1;
		codeArr[i + size-6][8].isFunctionGraph = 1;
	}

	/*��λͼ��*/
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
	/*��ʽ��Ϣ����*/
	for (int i = 0; i <9; i++)
	{
		codeArr[9][i + 1].isFunctionGraph = 1;
		codeArr[i + 1][9].isFunctionGraph = 1;
		codeArr[9][i + size - 7].isFunctionGraph = 1;
		codeArr[i + size -7][9].isFunctionGraph = 1;
	}

	/*С�ڵ�*/
	codeArr[size - 7][9].isFunctionGraph = 1;
	codeArr[size - 7][9].value = 1;
}
void QRcode::printCode()
{
	HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	setfontsize(hout, L"��������", 16);
	int size = 17 + 4 * version;//��ʽ
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

/*��ȡ�����ַ������ַ�������Ҫ�����������~   //������Ҫ�ˣ�ǰ�������ʶ�Դ���
int QRcode::countNumOfWord()
{
	int length = 0;
	for (int i = 0; i < (signed)strlen(info);)
	{
		if ((int)info[i] < 0)//GB2312�У������ַ��ĵ�һ���ֽڵ�ʮ������С���㣻����֮���Խ����һ���ֽ�
			i += 2;
		else
			i++;
		length++;
	}
	return length;
}*/  


/*��������=ģʽָʾ��+����ָʾ��+�����ԭ��+��λ*/
void QRcode::dataCharToByteArr()
{
	void addBits(char *dataStream, int maxNumBit);
	
	/*���ݱ����ԭ��*/
	int len = strlen(info);
	char *utf8 = (char *)malloc(sizeof(char)*len * 2);
	if (utf8 == NULL)
		exit(LOVERFLOW);
	GBKToUTF8(info, utf8);//GB->UTF8

	int length = strlen(utf8);
	char charCountIndicator[CHARCOUNTLENGTH + 1];//1-9�汾�ļ���ָʾ��Ϊ��λ��
	charCountIndicator[CHARCOUNTLENGTH] = '\0';//β�����Ҫ��,�ַ����飬���ַ������ӵȲ���ʱ��β��~~~~~
	for (int i = CHARCOUNTLENGTH - 1; i >= 0; i--)
	{
		charCountIndicator[i] = (char)((length & 0x0001) + '0');//ת���ɶ�������
		length >>= 1;
	}

	char *binary = (char *)malloc(sizeof(char)*len * 16);//������ݱ����ԭ��
	if (binary == NULL)
		exit(LOVERFLOW);
	char a;//��ʱ��������ÿ���ֽڴ洢Ϊshort��~
	int bitCount = 8;
	for (int i = 0; utf8[i]; i++)
	{
		a = utf8[i];
		for (int j = 0; j < 8; j++)
		{
			binary[--bitCount] = char((a & 0x0001)+'0');//������ȡ�������Ҫ����洢�ſ��Եõ���ȷ˳��
			a >>= 1;
		}
		bitCount += 16;
	}
	binary[bitCount - 8] = '\0';//������Ҫ��ǿ�����ַ��������ַ�������ʱβ�����Ҫ��
	free(utf8);

	char modeIndicator[5] = "0100";//������ģʽָʾ��Ϊ0100
	//char modeIndicator[5] = "0001";//�Ļ���������Ϊ�˲���8796
//	int length = countNumOfWord();//�ַ��������硰us�ң�,����5���ַ�
	

	strcpy(dataStream, modeIndicator);
	strcat(dataStream, charCountIndicator);//��������һ���ֽڵĳ��ȡ�����֪��������~��β�����ˡ�����
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
	int numZero = 0;//��λ����
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
	/*�����ַ����������*/
	for (int i = 0; i < x; i++)
		if (i % 2)
			strcat(dataStream, addString2);
		else
			strcat(dataStream, addString1);
}

void QRcode::setInfoPolymon(PolyLink *InfoPolyLhead)//������Ϣ����ʽ
{
	int expn[5] = { 7,10,15,20,26 };//��ͬ�İ汾��Ӧ�ľ���������
	int numByteOfData = NumBitOfData / 8;//���ݱ�����ֽ�����
	int expnValue = numByteOfData + expn[version - 1];//�������ɶ���ʽ����������Ϣ����ʽ����ߴ�����һ
	PolyLink p =*InfoPolyLhead;

	/*С�ģ���Ϣ����ʽ���������Ҫ������ϵ�����ϵ��Ϊ0����Ҳ��Ҫ������
	��Ϊ�ڽ��εĹ����У����������������㣬�����������ģ�Ҳ�ض��������*/
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
			s->coef = 0;//����Ϣ����ʽ��������֮�󣬵ʹε�ϵ��Ϊ��
		s->expn = --expnValue;
		p->next = s;
		p = s;
	}
	p->next = NULL;
}


void QRcode::setGeneratorPolymon(PolyLink *GPolyLhead)//�������ɶ���ʽ�ĵ�������(ϵ��ΪA^n�Σ�������洢����n����������)
{
	/*����ѡ��İ汾�ϵͣ����ɶ���ʽ��ϵ���Ԧ�^n����ʽ�е�n����ֱ�Ӵ������洢�������ڴ洢��������*/
	//�ô��Ƿ���Ҫ�洢���������迼�ǣ������Ǻţ���Ϊ���ֺ���ֻ�õ�����Щn&&&&&&&
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
	int expn[5] = { 7,10,15,20,26 };//�汾��Ӧ�����ɶ���ʽ����
	PolyLink p = *GPolyLhead;
	int hExpn = expn[version - 1];//���ɶ���ʽ����������
	for (int i = 0; i <= hExpn; i++)
	{
		PolyLink s = (PolyLink)malloc(sizeof(Polymon));
		if (s == NULL)
			exit(LOVERFLOW);
		s->coef = generatorPolyExpn[i];//���ɶ���ʽϵ�����д���Ǧ��Ĵ���
		s->expn = hExpn-i;//�����ߵ�����ǰ��
		p->next = s;
		p = s;
	}
	p->next = NULL;
}


void QRcode::errorCode()
{
	PolyLink GPolyLhead = (PolyLink)malloc(sizeof(Polymon));//���ɶ���ʽ������
	if (GPolyLhead == NULL)
		exit(LOVERFLOW);
	setGeneratorPolymon(&GPolyLhead);

	PolyLink InfoPLhead = (PolyLink)malloc(sizeof(Polymon));//������Ϣ����ʽ����
	if (InfoPLhead == NULL)
		exit(LOVERFLOW);
	setInfoPolymon(&InfoPLhead);

	/*PolyLink pp = GPolyLhead->next;
	cout << "���ɶ���ʽΪ" << endl;
	for (int i = 0; pp; i++)
	{
		cout << pp->coef << "*X^" << pp->expn << endl;
		pp = pp->next;
	}

	pp = InfoPLhead->next;
	cout << "��Ϣ����ʽΪ" << endl;
	while (pp)
	{
		cout << pp->coef << "*X^" << pp->expn << endl;
		pp= pp->next;
	}*/

	PolyLink p = InfoPLhead->next;
	PolyLink q = GPolyLhead->next;
	int infoHExpn = p->expn;
	int GHExpn = q->expn;
	for (int i = 0; i<=infoHExpn - GHExpn ; i++)//һֱ���ж���ʽ�����ֱ��ʣ������Ϊ ���ɶ���ʽ�Ĵ���
	{
		if (p->coef == 0)
		{
			p = p->next;
			continue;//���ϵ���Ѿ���0����ֱ�ӹ�
		}
		int addExpn = AnToN(p->coef);//����Ϣ����ʽ������ϵ�����ɦ�^n����ʽ��ʹ�����ɶ���ʽϵ�����ʱ���Լ򻯳�ָ����ӣ������������
		q = GPolyLhead->next;
		PolyLink s = p;
		for (int j = 0; j <= GHExpn; j++)
		{
			//����������㷨������ʽ������ľ��裬����ԭ�����ϳ�����
		    //http://tiierr.xyz/%E4%BA%8C%E7%BB%B4%E7%A0%81-%E6%9C%80%E7%BB%88%E7%BC%96%E7%A0%81/ ϵ�в���
			int ceofTmp = q->coef + addExpn;
			if (ceofTmp > 255)
				ceofTmp = ceofTmp % 255;

			s->coef = (s->coef) ^( nToAn(ceofTmp));
			s = s->next;
			q = q->next;
		}
		InfoPLhead->next = p->next;
		free(p);//��ʱ�ͷ���Ϣ����ʽ���
		p = InfoPLhead->next;
	}
	
	/*p = InfoPLhead->next;
	cout << "��������ʽΪ" << endl;
	while (p)
	{
		cout << p->coef << "*X^" << p->expn << endl;
		p = p->next;
	}*/
	
	/*��ʮ���ƾ�����ת����λ��*/
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
	
	/*�ͷ����ɶ���ʽ������*/
	PolyLink w = GPolyLhead, ww;
	while (w)
	{
		ww = w->next;
		free(w);
		w = ww;
	}
	/*�ͷ���Ϣ����ʽ������*/
    w = InfoPLhead;
	while (w)
	{
		ww = w->next;
		free(w);
		w = ww;
	}
	
	/*cout << "������������Ϊ" << endl;
	int i = 0;
	while (errorCodeStream[i])
	{
		cout << (int)errorCodeStream[i];
		i++;
		if (i % 8 == 0)
			cout << endl;
	}*/
}

inline int QRcode::nToAn(int n)//����ƪ���϶̣������������Ч��
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
	if (version != 1)//ͬ����1-5�汾������ֱ�Ӷ��ƣ�����Ľ������ù�ʽ���м���
		strcat(codeStream, "0000000");
	char *p = codeStream;
	int size = 17 + 4 * version;
	for (int j = size - 1; j > 0;j-=2 )
	{
		if ((j / 2) % 2 == 0)
			for (int i = size; i > 0; i--)//���µ���
			{
				if (codeArr[i][j + 1].isFunctionGraph == 0)//Խ����������
				{
					if (*p == '1')//Ԫ��valueֵ��ʼ��Ϊ0����*p=��0���ǲ���Ҫ�����
						codeArr[i][j + 1].value = 1;
					p++;//ֻҪ�����ǹ�������ָ�붼��Ҫ����
				}
				if (codeArr[i][j].isFunctionGraph == 0)
				{
					if (*p == '1')
						codeArr[i][j].value = 1;
					p++;
				}
			}
		else if ((j / 2) % 2 == 1)//���ϵ���
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
	int size = 17 + 4 * version;//��ʽ
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
	int size = 17 + 4 * version;//��ʽ
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
	score += 36;//λ��̽����й̶���36��
	return score;
}
int QRcode::maskEvaluate3(QRcodeNode tmp[N][N])
{
	int score = 0;
	int size = 17 + 4 * version;//��ʽ
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
	int size = 17 + 4 * version;//��ʽ
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
	int size = 17 + 4 * version;//��ʽ
	QRcodeNode tmp[N][N];
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
			tmp[i][j].isFunctionGraph = codeArr[i][j].isFunctionGraph;

	int minScore = 0, score = 0;
	strcpy(formatInfo, "111011111000100");
	/*����ģʽ0 ��Ӧ��ʽ��Ϣ*/
	setFormatInfo(tmp, formatInfo);
	for (int i = 0; i < size + 2; i++)
		for (int j = 0; j < size + 2; j++)
		{
			if (tmp[i][j].isFunctionGraph == 0 &&(i + j) % 2 == 0)
				tmp[i][j].value = !(codeArr[i][j].value);//~�����ã�Ҫ�ã�
			else
				tmp[i][j].value = codeArr[i][j].value;
		}
	score = maskEvaluate1(tmp) + maskEvaluate2(tmp) + maskEvaluate3(tmp) + maskEvaluate4(tmp);
	maskId = 0;
	minScore = score;

	/*����ģʽ1*/
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

	/*����ģʽ2*/
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

	/*����ģʽ3*/
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

	/*����ģʽ4*/
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

	/*����ģʽ5*/
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
		strcpy(formatInfo, "110001100011000");//���������+�������+����λ��������101010000010010��������
	}

	/*����ģʽ6*/
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

	/*����ģʽ7*/
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
	int size = 17 + 4 * version;//��ʽ
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

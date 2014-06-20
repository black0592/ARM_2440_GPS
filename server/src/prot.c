#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "prot.h"
#include "logc.h"

#define COMMTYPE_BYTENUM 1   //���������ֽ���
#define COMM_BYTENUM 1		//�������ֽ���
#define HEAD_BYTENUM 1		//��ͷ�ֽ���
#define END_BYTENUM 1		//��β�ֽ���
#define DATALENTH_BYTENUM 4	//���ݳ����ֽ���
#define CHECKSUM_BYTENUM 1	//У����ֽ���

unsigned char getXORSum(unsigned char *src, int checkSize);

int packet(unsigned char *p, unsigned char *data, int dataLenth, unsigned char commType, unsigned char comm)  
{
	unsigned char HEAD = 0x7E;
	unsigned char END = 0x7F;
	unsigned char ESCAPE_7D = 0x7d;
	unsigned char ESCAPE_5D = 0x5d;
	unsigned char ESCAPE_5E = 0x5e;
	unsigned char ESCAPE_5F = 0x5f;

	unsigned char ESCAPE_X;

	int checkSize = 0;
	int escapeCount = 0;	//���������ݵĳ���
	int escapeSize = 0;		
	int tempCount = 0;   //������ֹת���������
	unsigned char checkSum;		//У���
	unsigned char *checkData = NULL;
	unsigned char *escapeData = NULL;
	unsigned char *escapeTemp = NULL;
	unsigned char *escapeData_t = NULL;
	unsigned char *checkData_t = NULL;
	unsigned char escapeByte;

	//checkSizeΪ�������͡�����Ϣ��������ܳ���
	checkSize = COMMTYPE_BYTENUM + COMM_BYTENUM +  DATALENTH_BYTENUM + dataLenth;
	if( NULL == (checkData = (unsigned char *)malloc(sizeof(unsigned char) * checkSize)) )
	{
		LOG(LOG_NOTICE, "�����ڴ�ʧ�ܣ�");
		return 0;
	}

	memset(checkData, 0, checkSize);
	//���������͡�����Ϣ���������䵽checkData��
	checkData_t = checkData;
	memcpy(checkData_t, &commType, COMMTYPE_BYTENUM);
	checkData_t += COMMTYPE_BYTENUM;
	memcpy(checkData_t, &comm, COMM_BYTENUM);
	checkData_t += COMM_BYTENUM;
	memcpy(checkData_t, &dataLenth,  DATALENTH_BYTENUM);
	checkData_t +=  DATALENTH_BYTENUM;
	memcpy(checkData_t, data, dataLenth);

	//checkSumΪ�������͡�����Ϣ�����ݵ��ֽ�����
	checkSum = getXORSum(checkData, checkSize);
	


//����ת����жϺʹ���
	escapeSize = checkSize + 1;  //escapeSizeΪ�������͡���У��͵����ݵĳ���
	if( NULL == (escapeData = (unsigned char *)malloc(sizeof(char) * escapeSize)) )
	{
		LOG(LOG_NOTICE, "�����ڴ�ʧ�ܣ�");
		return 0;
	}
	//���������͡���У���������䵽escapeData�У����������ת�崦��
	memcpy(escapeData, checkData, checkSize);
	memcpy(escapeData + checkSize, &checkSum, 1);

	//���Ƚ�HEADֵ0x7E����p��
	memcpy(p, &HEAD, 1);
	escapeCount = 1;


//Ȼ��escapeData������һһ����ת���Ⲣ��䵽p��
	escapeTemp = escapeData;
	escapeByte = *escapeTemp;
	escapeData_t = p + 1;

	while(tempCount < escapeSize)//tempCountΪescapeData���Ѿ���ת��������ݳ���
	{
		//0x7fת��
		if(END == escapeByte)
		{
			ESCAPE_X = ESCAPE_5F;
		}
		//0x7eת��
		else if(HEAD == escapeByte)
		{
			ESCAPE_X = ESCAPE_5E;
		}
		//0x7dת��
		else if(ESCAPE_7D == escapeByte)
		{
			ESCAPE_X = ESCAPE_5D;
		}
		else
		{	//��������Ҫת�壬ֱ����䵽p������
			memcpy(escapeData_t, &escapeByte, 1);
			escapeData_t++;

			tempCount++;

			escapeTemp++;
			escapeByte = *escapeTemp;
			escapeCount++;
			continue;
		}
		//������Ҫת��Ĳ���
		memcpy(escapeData_t, &ESCAPE_7D, 1);
		escapeData_t++;
		memcpy(escapeData_t, &ESCAPE_X, 1);
		escapeData_t++;

		tempCount++;

		escapeTemp++;
		escapeByte = *escapeTemp;
		escapeCount += 2;
	}

	//���ENDֵ0x7F����p��
	memcpy(escapeData_t, &END, 1);
	escapeCount++;

	free(escapeData);
	free(checkData);
	
	return escapeCount;
}


//������p�ǽ�������ݴ�ŵĻ����ַ��data��Ҫ���н�������İ��ĵ�ַ
//		dataLenthΪdata���ݵĳ���
//���أ��ɹ����ؽ�������ݵĳ���checkSize��ʧ�ܷ���0.

int unpacket(unsigned char *p, unsigned char *data, int dataLenth) 
{
	unsigned char HEAD = 0x7E;
	unsigned char END = 0x7F;
	unsigned char ESCAPE_7D = 0x7d;
	
	unsigned char *escapeData = NULL;
	unsigned char *escapeTemp = NULL;
	unsigned char *dataTemp = NULL;
	unsigned char *escapeData_t = NULL;
	unsigned char *checkData = NULL;
	unsigned char *checkTemp = NULL;
	unsigned char checkSum;
	unsigned char escapeByte;
	unsigned char ESCAPE_X;

	int escapeCount = 0;
	int checkSize = 0;
	int tempCount = 0;
	
	tempCount = dataLenth-HEAD_BYTENUM-END_BYTENUM;

	if( NULL == ( escapeData=(unsigned char*)malloc(sizeof(unsigned char) * tempCount) ) )
	{
			LOG(LOG_NOTICE, "�����ڴ�ʧ�ܣ�");
			return 0;
	}


	dataTemp = data;
	dataTemp++;//ֱ�Ӻ��԰�ͷHEAD
	/*ȥ����ͷ��β*/
	memcpy(escapeData, dataTemp, tempCount);

	/*��ת�崦��*/	
	escapeTemp = escapeData;
	escapeByte = *escapeTemp;
	escapeData_t = p;
	while(tempCount > 0)
	{
		if(0x7d == escapeByte)
		{
			escapeTemp++;
			escapeByte = *escapeTemp;
			if(0x5d == escapeByte)
			{
				ESCAPE_X = ESCAPE_7D;
			}
			else if(0x5e == escapeByte)
			{
				ESCAPE_X = HEAD;
			}
			else if(0x5f == escapeByte)
			{
				ESCAPE_X = END;
			}
			else
			{
				LOG(LOG_NOTICE, "��ת�崦���У����ַ��ת����ִ���");
				return 0;
			}
			memcpy(escapeData_t, &ESCAPE_X, 1);
			escapeData_t++;
			escapeCount++;	
			tempCount--;	
		}
		else
		{
			memcpy(escapeData_t, &escapeByte, 1);
			escapeData_t++;
			escapeCount++;
		}
		
		tempCount--;
		escapeTemp++;
		escapeByte = *escapeTemp;
	}

	checkTemp = p;
	checkSize = escapeCount - CHECKSUM_BYTENUM;

	if( NULL == (checkData = (unsigned char *)malloc(sizeof(unsigned char) * checkSize)) )
	{
		LOG(LOG_NOTICE, "�����ڴ�ʧ�ܣ�");
		return 0;
	}

	memcpy(checkData, checkTemp, checkSize);

	checkTemp += checkSize;
	checkSum = *checkTemp;

	if( checkSum != getXORSum(checkData, checkSize) )
	{
		LOG(LOG_NOTICE, "У��Ͳ�һ�£�");
		return 0;
	}

	free(escapeData);
	free(checkData);

	return checkSize;

}

/*��������*/
unsigned char getXORSum(unsigned char *src, int checkSize)
{
  unsigned char UcRc;
  unsigned char *srcTemp;
	int count = 0;
	 
  UcRc = 0;
  srcTemp = src;

  while(checkSize > count)      
  {
       UcRc ^= *srcTemp;
       srcTemp++;
	   count++;
   }

   return UcRc;
}

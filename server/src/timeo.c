/**
 * Copyright(c) 2011, Free software organization.
 * All Rights Reserved.
 * 
 * @author kezf<kezf@foxmail.com>
 * @file timeo.c
 * @brief ʱ�䴦��������
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include "timeo.h"

/**
 * @brief ��ָ����ʽ���ʱ���ַ�����
 * 
 * @param pstr ����ַ������塣
 * @param size ����ַ��������С��
 * @param format ʱ�����ڸ�ʽ��yyyy: 4λ�꣬yy: 
 *  			 2λ�꣬mm���·ݣ�dd�����ڣ�hh��24Сʱ�ƣ�mi�����ӣ�ss����,
 *  			 ms: ����, us: ΢�
 *
 * @return ʵ������ַ����ĳ��ȡ�
 */
int sftime(char *pstr, int size, const char *format)
{
	char *p;
	int iRet = 0;
	struct tm ts;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &ts);
	p = (char *)format;
	while (*p != '\0') {
		if (*p == 'y' && *(p + 1) == 'y') {
			if (*(p + 2) == 'y' && *(p + 3) == 'y') {
				if (iRet + 4 > size) {
					errno = EINVAL;
					return -1;
				}
				sprintf(pstr, "%04d", ts.tm_year + 1900);
				p += 4;
				pstr += 4;
				iRet += 4;
			} else {
				if (iRet + 2 > size) {
					errno = EINVAL;
					return -1;
				}
				sprintf(pstr, "%02d", (ts.tm_year + 1900) % 100);
				p += 2;
				pstr += 2;
				iRet += 2;
			}
		} else if (*p == 'm' && *(p + 1) == 'm') {
			if (iRet + 2 > size) {
				errno = EINVAL;
				return -1;
			}
			sprintf(pstr, "%02d", ts.tm_mon + 1);
			p += 2;
			pstr += 2;
			iRet += 2;
		} else if (*p == 'd' && *(p + 1) == 'd') {
			if (iRet + 2 > size) {
				errno = EINVAL;
				return -1;
			}
			sprintf(pstr, "%02d", ts.tm_mday);
			p += 2;
			pstr += 2;
			iRet += 2;
		} else if (*p == 'h' && *(p + 1) == 'h') {
			if (iRet + 2 > size) {
				errno = EINVAL;
				return -1;
			}
			sprintf(pstr, "%02d", ts.tm_hour);
			p += 2;
			pstr += 2;
			iRet += 2;
		} else if (*p == 'm' && *(p + 1) == 'i') {
			if (iRet + 2 > size) {
				errno = EINVAL;
				return -1;
			}
			sprintf(pstr, "%02d", ts.tm_min);
			p += 2;
			pstr += 2;
			iRet += 2;
		} else if (*p == 's' && *(p + 1) == 's') {
			if (iRet + 2 > size) {
				errno = EINVAL;
				return -1;
			}
			sprintf(pstr, "%02d", ts.tm_sec);
			p += 2;
			pstr += 2;
			iRet += 2;
		} else if (*p == 'm' && *(p + 1) == 's') {
			if (iRet + 3 > size) {
				errno = EINVAL;
				return -1;
			}
			sprintf(pstr, "%03ld", tv.tv_usec / 1000);
			p += 2;
			pstr += 3;
			iRet += 3;
		} else if (*p == 'u' && *(p + 1) == 's') {
			if (iRet + 6 > size) {
				errno = EINVAL;
				return -1;
			}
			sprintf(pstr, "%06ld", tv.tv_usec);
			p += 2;
			pstr += 6;
			iRet += 6;
		} else {
			if (iRet + 1 > size) {
				errno = EINVAL;
				return -1;
			}
			*pstr = *p;
			p++;
			pstr++;
			iRet++;
		}
	}
	return iRet;
}


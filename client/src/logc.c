/**
 * @file logc.c
 * @brief ����־�ӿڡ�
 *
 * ֱ�������־����LOG�ӿڣ���Ҫָ����־�ļ�������LOGN�ӿڣ���Ҫ��չ��־�ӿ�����vLOGN�ӿڡ�
 */
#include <stdio.h>
#include <string.h>
#include "timeo.h"
#include "logc.h"

/**
 * @brief ��־�ӿڡ�
 * 
 * @param name ��־�ļ����ơ�
 * @param level ��־�ȼ���
 * @param format ��־���ݸ�ʽ��
 * @param args ��ʽ������
 */
void vLOGN(const char *name, const int level, const char *file, const int line, const char *format, va_list args)
{
	FILE *fp;
	char buf[LOG_SIZE + 1], FilePath[LOG_PATH_MAX + 1], file_name[LOG_NAME_MAX + 1];
	char ts[50];
	
	if (name == NULL) {
		strcpy(file_name, "log");
	} else {
		strcpy(file_name, name);
	}
	memset(ts, 0, sizeof(ts));
	sftime(ts, sizeof(ts), "yyyy-mm-dd");
	sprintf(FilePath, "%s%s.%s.log", LOG_PATH, file_name, ts);
	vsprintf(buf, format, args);
	fp = fopen(FilePath, "a+");
	if (fp == null) {
		return;
	}
	memset(ts, 0, sizeof(ts));
	sftime(ts, sizeof(ts), "yyyy-mm-dd hh:mi:ss.ms");
	fprintf(fp, "[%s] %s (%s:%d) - %s\n", priorities[level], ts, file, line, buf);
	fflush(fp);
	fclose(fp);
}

/**
 * @brief д��־��
 * 
 * @param name ��־�ļ����ơ�
 * @param level ��־�ȼ���
 * @param format ��־���ݸ�ʽ��
 * @param ... ��ʽ������
 */
void _LOGN(const char *name, const int level, const char *file, const int line, const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	vLOGN(name, level, file, line, format, args);
	va_end(args);
}

/**
 * @brief д��־��
 * 
 * @param level ��־�ȼ���
 * @param format ��־���ݸ�ʽ��
 * @param ... ��ʽ������
 */
void _LOG(const int level, const char *file, const int line, const char *format, ...)
{
	va_list args;
	
	va_start(args, format);
	vLOGN(NULL, level, file, line, format, args);
	va_end(args);
}


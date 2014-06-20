#ifndef __CONFIG_H__    /*�����ļ�����һЩ����*/
#define __CONFIG_H__

#define SERVE_PORT 	  		8000	/*�����Ҫ�󶨵Ķ˿ں�*/
#define THREAD_ACCEPT_MAX	5		/*�������̳߳��е��߳���Ŀ*/
#define CLIENT_ID_MAX 		12      /*�ͻ���ID  ����󳤶ȣ�����������*/
#define CONN_NUM_MAX  		256		/*����ͬʱ�������ӵ����ͻ��˸���*/
#define ACCEPT_THREAD_MAX 	3		/*�����ļ����׽����̳߳���������������߳���*/
#define DATA_MAX_SIZE 		1024	/*���ݻ�����������ֽ���*/
#define MSG_HEART_BEAT 		3		/*����������������*/
#define HEART_BEAT_TIME		5		/*����ʱ��������λΪs*/
#define HEART_MAX_CNT		3		/*����������δ�ɹ���������*/
#define DEBUG				1   
#define CQUEUE_SIZE			4096	/*Ϊÿ���ͻ��˷����ѭ�����д�С*/

/*��������*/
#define COMM_TYPE_ALARM		0x00				/*��������*/
#define COMM_TYPE_SERVE		0x10				/*��������*/
#define COMM_TYPE_HEARBEAT	0x30				/*������*/
/*����������*/
#define COMM_ALARM_CARJACKING		0x01		/*�ٳ�����*/
#define COMM_ALARM_TIREPRESURE		0x02		/*̥ѹ����*/
#define COMM_ALARM_BEYONGD_REGION	0x03		/*Խ�����򱨾�*/
#define COMM_ALARM_CARJACKING_C		0x04		/*����ٳ�����*/
#define COMM_ALARM_TIREPRESURE_C	0x05		/*���̥ѹ����*/
#define COMM_ALARM_BEYONGD_REGION_C	0x06		/*���Խ�����򱨾�*/

/*����������*/
#define COMM_SERV_GPS				0x11		/*�ϴ�GPS  ���ݰ�*/
#define COMM_SERV_LOCK				0x12		/*����ָ��*/
#define	COMM_SERV_UNLOCK			0x13		/*����ָ��*/
#define COMM_SERV_BITMAP			0x14		/*�ϴ�ͼƬ*/

#define HEAD  0x7E				/*��ͷ*/
#define END   0x7F				/*��β*/

#define DATA_START				6	/*��ͨ���ݰ�������ʼλ��*/
#define FILE_NAME_LEN_START		6	/*ͼƬ���������ļ������ȵ�������ʼ��*/
#define FILE_NAME_START			10	/*ͼƬ�������򣬴���ļ�����ʼλ��*/

#define BUFFER_SIZE 			1024
#define NAME_MAX_SIZE			50

#endif /* __CONFIG_H__ */

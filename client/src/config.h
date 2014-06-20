#ifndef __CONFIG_H__    /*�����ļ�����һЩ����*/
#define __CONFIG_H__

#define CLIEN_ID	  		"12345678901" 	/*���ͻ���ID*/
#define SERVE_PORT 	  		8000			/*�����Ҫ�󶨵Ķ˿ں�*/
#define CLIENT_ID_MAX 		12        		/*�ͻ���ID  ����󳤶ȣ�����������*/
#define MAXSLEEP 			128				/*connect ��ʱ�������˯��ʱ��*/	
#define HEART_BEAT_TIME		5				/*����ʱ��������λΪs*/
#define HEART_MAX_CNT		3				/*����������δ�ɹ���������*/
#define DATA_MAX_SIZE 		1024
#define DEBUG				1

#define GPS_UPLOAD_TIME		15				/*��ʱ�ϴ�GPS  ���ݰ���ʱ����*/
#define MSG_HEART_BEAT 		1
#define MSG_IMAGE_UPLOAD    2

#define DEV_BUTTON_CHAR_PATH	"/dev/buttons"
#define DEV_BUTTON_INPUT_PATH 	"/dev/input/event1"
#define DEV_PWN_PATH 			"/dev/mini2440_pwm"
#define DEV_LED_PATH 			"/dev/mini2440_led"

#define IS_MINI2440			1

#define PWM_IOCTL_SET_FREQ		1
#define CHAR_DEV_DRIVE				0					/*���������Ƿ����ַ�����*/
#define INPUT_DEV_DRIVE			1					/*���������Ƿ���������ϵͳ*/

#define SERVER_ADDR		"219.229.129.235"	
#define CQUEUE_SIZE		4096						/*��Ϣ���е���󳤶�*/

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

#define BUFFER_SIZE 1024
#define NAME_MAX_SIZE	50

#endif /* __CONFIG_H__ */

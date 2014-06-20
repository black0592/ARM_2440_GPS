#include "socket.h"
#include "wrap.h"
#include "logc.h"
#include "alarm.h"
#include "messageHandler.h"
#include "circleQueue.h"
#include "fileo.h"

#include <string.h> 		/*strncpy*/
#include <stdlib.h>			/*exit*/
#include <errno.h>			/*errno*/
#include <stdio.h>			/*perror*/
#include <unistd.h>			/*read*/
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


void client_init();
void client_uninit();
void client_connect_main(int *socketfd);
void *client_recv_main(void *param);

struct sockaddr_in servaddr;
volatile fd_set allset;
int led_fd = -1;
volatile int led1_status, led2_status;

int main(int argc, char *argv[]){	
	pthread_t pid_recv, pid_messgeHandler;
	pthread_t pid_alarm;
	
	client_init();
	client_connect_main(&socket_fd_ctd.socketfd);
	heart_beat_client(socket_fd_ctd.socketfd);     					/*����������*/
	client_gps_main();							   					/*����GPS  ���ݰ���ʱ�ϴ�*/
	
#if IS_MINI2440
	
	Pthread_create(&pid_alarm, NULL, init_alarm_func, NULL);		/*�����̼߳������־�����Ϣ*/
#endif
	Pthread_create(&pid_recv, NULL, client_recv_main, NULL);		/*�����߳̽�����Ϣ*/
	Pthread_create(&pid_messgeHandler, NULL, messageHandler, NULL);

	Pthread_join(pid_recv, NULL);
	Pthread_join(pid_messgeHandler, NULL);
	
#if IS_MINI2440
	Pthread_join(pid_alarm, NULL);
#endif
	client_uninit();
	return 0;
}

/**
 * @brief	���пͻ���һЩ��ʼ���������ļ���������ʼ����
 */
void client_init(){
	LOG(LOG_DEBUG, "client init...\n");
#if	IS_MINI2440
	if(led_fd < 0)
		led_fd = open(DEV_LED_PATH, O_RDWR | O_NONBLOCK);
	if(led_fd < 0){			//�򲻿������ǿ������У�ֻ��LED  �ƹ��ܲ�����
		LOG(LOG_ERROR, "LED�Ʋ�����:%s", strerror(errno));
	}
#endif
	strncpy(socket_fd_ctd.client_id, CLIEN_ID, CLIENT_ID_MAX);
	socket_fd_ctd.client_regisered = 0;
	socket_fd_ctd.heartbeat_cnt = 0;
	socket_fd_ctd.socketfd_status = -1;
	socket_fd_ctd.status = 0;
	Pthread_mutex_init(&(socket_fd_ctd.socketfd_mt), NULL);

	socket_fd_ctd.Q = Cqueue_init(CQUEUE_SIZE);			/*��ʼ����Ϣ����*/
}

void client_uninit(){
	Pthread_mutex_destroy(&socket_fd_ctd.socketfd_mt);
	Close(socket_fd_ctd.socketfd);
	socket_fd_ctd.status = -1;

	DestroyQueue(socket_fd_ctd.Q);		/*������Ϣ����*/
	
#if	IS_MINI2440
	led_fd = -1;
#endif
}

void client_connect_main(int *socketfd){
	int n;
	char buf[5];
	
	if((*socketfd = c_socket(SERVE_PORT)) < 0){
		LOG(LOG_ERROR, "fail to create socket:%s",strerror(errno));
		exit(-1);
	}
	LOG(LOG_DEBUG, "send the client ID...\n");
	writen(*socketfd, socket_fd_ctd.client_id, CLIENT_ID_MAX);
	/*
		�������Է���˵�ȷ������
	*/
	if((n = read(*socketfd, buf, 5)) < 0){			/*��������*/
		LOG(LOG_ERROR,"read error:%s", strerror(errno));
		exit(-1);
	}else{
		if(strncmp(buf, "busy", 5) == 0){					/*��������æ*/
			
		}else if(strncmp(buf, "ok", 5) == 0){			/*ע��ɹ�*/
			LOG(LOG_DEBUG, "register success...\n");
		}else{											/*δע��*/
			printf("���ĳ��ƺŻ�δע�ᣬ����ʹ��������!\n");
			client_uninit();
			exit(-1);									
		}
	}
}

void *client_recv_main(void *param){
	fd_set rset;
	int nready, socketfd, n;
	char data[DATA_MAX_SIZE];
	struct timeval rto;

	socketfd = socket_fd_ctd.socketfd;
	FD_ZERO(&allset);
	FD_SET(socketfd, &allset);
	
	for(; ;){
		rset = allset;
		rto.tv_sec = 5;
		rto.tv_usec = 0;

		if(socket_fd_ctd.status == -1){     //�������Ͽ����ӣ��������ӷ�����
			
			client_connect_main(&socket_fd_ctd.socketfd);
			socketfd = socket_fd_ctd.socketfd;
			client_init();
			FD_SET(socketfd,&allset);
		}
		if((nready = select(socketfd+1, &rset, NULL, NULL, &rto)) < 0){
			if(errno == EINTR)
				continue;
			else{
				LOG(LOG_ERROR,"select error:%s", strerror(errno));
				exit(-1);
			}	
		}else if(nready == 0){						/*��ʱ*/
			
		}else{
			if(FD_ISSET(socketfd, &rset)){
				bzero(data, DATA_MAX_SIZE);
				if((n = read(socketfd, data, DATA_MAX_SIZE)) == 0){
					/*�������ر����׽ӿڣ��������ӷ�����*/
					LOG(LOG_DEBUG, "the server has been closed the sock,we will reconnect again...\n");
					printf("����˶Ͽ����ӣ�������������...\n");
					
					client_uninit();
					FD_ZERO(&allset);
				}else if(n < 0){		/*���ִ���*/
					
				}else{
					int ret = 0;
					/*����Ϣ������Ϣ����*/
					while((ret = EnterQueue(socket_fd_ctd.Q, data, n)) == 0);		/**/
				}
			}
		}
	}
}
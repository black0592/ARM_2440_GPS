#include "socket.h"
#include "my_pthread.h"
#include "wrap.h"
#include "logc.h"
#include "messageHandler.h"
#include "menu.h"

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>    /*fd_set*/
#include <unistd.h>
#include <time.h>			/*struct timeval*/


pthread_mutex_t	mlock = PTHREAD_MUTEX_INITIALIZER;     /*ʹ�����׽����̳߳�ͬ��*/
volatile fd_set allset;
volatile int maxfd, maxi;   /*maxfd������ļ���������maxi client��������±�*/

void server_init();
void server_uninit();
void server_accept_main(int socketfd);
void *server_recv_main(void *param);


int main(int argc, char **argv){
	int socketfd;
	int i;
	pthread_t server_recv_tid, message_handler_tid, menu_tid;
	
	server_init();
	if((socketfd = s_socket(SERVE_PORT)) < 0){
		exit(-1);
	}
	server_accept_main(socketfd);
	Pthread_create(&server_recv_tid, NULL, server_recv_main, NULL);
	Pthread_create(&message_handler_tid, NULL, messageHandler, NULL);
	heart_beat_server();
	Pthread_create(&menu_tid, NULL, showmenu, NULL);

	for(i = 0; i < THREAD_ACCEPT_MAX; i++)
		Pthread_join(tptr[i].thread_tid, NULL);
	Pthread_join(server_recv_tid, NULL);
	Pthread_join(message_handler_tid, NULL);
	Pthread_join(menu_tid, NULL);

	server_uninit();
	return 0;
}

/**
 * @brief	���з����һЩ��ʼ���������ļ���������ʼ����
 */
void server_init(){
	int i;
	
#if DEBUG
	printf("server init..\n");
	LOGN("server", LOG_DEBUG, "server init...");
#endif
	FD_ZERO(&allset);
	maxi = -1;
	maxfd = 0;
	for(i = 0; i < CONN_NUM_MAX; i++){
		client[i].socketfd = -1;
		client[i].socketfd_status = -1;	/*�ڳ��ζ�ʱ�����������û�иı�Ļ�������û���յ�������*/
		client[i].heart_beat_cnt = 0;
		Pthread_mutex_init(&client[i].socketfd_mt, NULL);
		client[i].Q = NULL;
	}
}

void server_uninit(){
	int i;

	for(i = 0; i < CONN_NUM_MAX; i++){
		Pthread_mutex_destroy(&client[i].socketfd_mt);
		DestroyQueue(client[i].Q);
	}
}

/**
 * @brief 	���������̳߳�
 */
void server_accept_main(int socketfd){
	int i ;

	tptr = Malloc(THREAD_ACCEPT_MAX* sizeof(Thread));
	
	for(i = 0; i < ACCEPT_THREAD_MAX; i++){
		thread_make(i, socketfd);
	}
}

/**
 * @brief 	ʹ��select  ʵʱ�����׼���ö����׽��������������н�����Ϣ
 			��������Ӧ���������ͽ��в�ͬ�Ĵ���
 */
void *server_recv_main(void *param){
	fd_set rset;
	int nready;		/*�洢select  ����ֵ����Ϣ*/
	int socketfd, i, n;
	unsigned char data[DATA_MAX_SIZE];
	struct timeval rto;

	for(; ;){
		rset = allset;
		rto.tv_sec = 5;
		rto.tv_usec = 0;
		if((nready = select(maxfd+1, &rset, NULL, NULL, &rto)) < 0 ){
			if(errno == EINTR)
				continue;					/*���жϵĻ�����*/
			else{
				perror("select error");
				exit(-1);
			}
		}else if(nready == 0){					/*��ʱ*/
				continue;
		}else{	
			for(i = maxi; i >= 0; i--){
				if((socketfd = client[i].socketfd) < 0)
					continue;

				if(FD_ISSET(socketfd, &rset)){			/*׼���ö�*/
					bzero(data, DATA_MAX_SIZE);		

					if((n = read(socketfd, data, DATA_MAX_SIZE)) == 0){		/*connection closed by client, recv FIN*/
						/*�ر��׽��֣��Ӽ������ַ������Ƴ�
							ͬʱ,���³�ʼ������ռ�õ�Ԫ
						*/
						printf("\n�ͻ�%s �ر�����!\n", client[i].client_id);
						LOGN("server", LOG_DEBUG, "the client %s closed!",client[i].client_id);
						
						Pthread_mutex_lock(&client[i].socketfd_mt);
						Close(socketfd);
						FD_CLR(socketfd, &allset);
						client[i].socketfd = -1;
						client[i].heart_beat_cnt = 0;
						client[i].socketfd_status = -1;
						bzero(client[i].client_id,CLIENT_ID_MAX);	
						DestroyQueue(client[i].Q);
						if(maxi == i){
							int k = i;
							maxi--;
							while(client[--k].socketfd < 0)
								maxi--;
						}
						Pthread_mutex_unlock(&client[i].socketfd_mt);							
					}else if(n < 0){					/*���ִ���*/
						
					}else{								/*������*/
						/*�����յ������ݴ�����Ӧ��ѭ��������*/
						int ret;
						
						while((ret = EnterQueue(client[i].Q, data, n)) == 0);						
					}
					if(--nready <= 0)/*֮ǰ��С�Ľ���仰�ŵ���if(FD_ISSET)֮�⣬�ͻ��˴���3�������в�������*/
						break;							/*û�пɶ���������*/
				}
			}
		}
	}     
}


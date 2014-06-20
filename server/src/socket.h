#ifndef __SOCKET_H__		/*���׽ӿڴ���ķ�װ*/
#define __SOCKET_H__

#include "config.h"
#include "circleQueue.h"

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct sockaddr SA;

typedef struct {
	//int is_connected;     			/*��client_id�Ƿ��Ѿ������˽������ӣ�1 ��ʾ�����ӣ�0 ��ʾδ����*/
	volatile int socketfd;					/*���ӹ������׽���������*/
	int socketfd_status;			/*����״̬��0 ��ʾ����������-1 ��ʾ�쳣*/
	int heart_beat_cnt;				/*������δ�ɹ�����*/
	pthread_mutex_t socketfd_mt;	/*������socket ����*/
	char client_id[CLIENT_ID_MAX];	/*�ͻ���ID*/
	Cqueue *Q;						/*ÿ���ͻ��˷���һ��ѭ�����д�����Ϣ*/
}socket_fd_conn;
socket_fd_conn client[CONN_NUM_MAX];	/*�洢�ͻ������ӹ�������Ϣ*/

extern int s_socket(int port);
extern ssize_t	readn(int fd, void *vptr, size_t n);
extern ssize_t	writen(int fd, const void *vptr, size_t n);
extern void heart_beat_server();
extern int send_data(int fd, int commtype, int comm,unsigned char *data, int len);

#endif /* __SOCKET_H__ */

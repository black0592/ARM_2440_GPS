#ifndef __SOCKET_H__		/*���׽ӿڴ���ķ�װ*/
#define __SOCKET_H__

#include "config.h"
#include "circleQueue.h"  /*struct Cqueue*/

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct sockaddr SA;

typedef struct {
	int socketfd;					/*�����ӵ��׽��֣�-1��ʾδ����*/
	int socketfd_status;			/*��ʾ�������Ƿ�ʧ��0  ��ʾ����������-1  ��ʾ�쳣*/
	pthread_mutex_t socketfd_mt;	/*������socket  ����*/
	int heartbeat_cnt;				/*������δ�ɹ�����*/
	int client_regisered;			/*�˿ͻ���ID  �Ƿ�ɹ�ע��*/
	volatile int status;						/*����״̬��0  ��ʾ����������-1  ��ʾ�쳣*/
	char client_id[CLIENT_ID_MAX];	/*�ͻ���ID*/
	Cqueue *Q;
}socket_fd_conn;
socket_fd_conn socket_fd_ctd;

extern ssize_t	writen(int fd, const void *vptr, size_t n);
extern ssize_t	readn(int fd, void *vptr, size_t n);
extern int c_socket(int port);
extern void heart_beat_client(int socketfd);
extern int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen);
extern void client_gps_main();
extern int send_data(int fd, int commtype, int comm, unsigned char *data, int len);

#endif /* __SOCKET_H__ */

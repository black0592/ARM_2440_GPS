#include "socket.h"
#include "wrap.h"
#include "mytools.h"
#include "logc.h"
#include "prot.h"

#include <errno.h>		/*errno*/
#include <unistd.h>		/*close*/
#include <string.h>		/*bzero*/
#include <signal.h>		/*SIGALRM*/

extern fd_set allset;
extern int maxi;


/**
 * @brief 		������׽��ֳ�ʼ������
 * @param type	�׽ӿ�����
 * @param addr	ָ���׽ӿڵ�ַ�ṹ��ָ��
 * @param alen	�׽ӿڵ�ַ�ṹ�Ĵ�С
 * @param qlen	ͬlisten  �ĵڶ�������
 * @return 		��ʼ������׽���������
 */
int init_server_socket(int type, const struct sockaddr *addr, socklen_t alen, int qlen){
	int socketfd;
	int err = 0;
	int opt;

	opt = 1;
	
	LOGN("server", LOG_DEBUG, "create sokcetfd");

	if((socketfd = socket(addr->sa_family, type, 0)) < 0)
		return -1;
	/*�ܿ�TIME_WAIT  ״̬*/
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
		err = errno;
		goto errout;
	}

	if(setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)) < 0){
		err = errno;
		goto errout;
	}
	if(bind(socketfd, addr, alen) < 0){
		err = errno;
		goto errout;
	}
	if(type == SOCK_STREAM || type == SOCK_SEQPACKET){
		if(listen(socketfd, qlen) < 0){
			err = errno;
			goto errout;
		}
	}
	return socketfd;

errout:
	close(socketfd);
	errno = err;
	return -1;
}

/**
 * @brief		����������ַ�ĳ�ʼ����������init_server_socket���н�һ���İ󶨵ȳ�ʼ������
 * @param port	������Ҫ�����Ķ˿ں�
 * @return 		�׽���������
 */
int s_socket(int port){
	int listenfd;
	struct sockaddr_in servaddr;
	socklen_t addr_len;

	addr_len = sizeof(struct sockaddr_in);
	bzero(&servaddr, addr_len);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	listenfd = init_server_socket(SOCK_STREAM, (SA *)&servaddr, addr_len, 20);

	return listenfd;
}

/**
 * @brief 		ȷ����һ���ļ��������ж�ȡn  ���ֽ�����
 */
ssize_t	readn(int fd, void *vptr, size_t n){					
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;
	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)				
				nread = 0;		/* and call read() again */			
			else				
				return(-1);		
		} else if (nread == 0)
			break;				/* EOF */		
		nleft -= nread;		
		ptr   += nread;	
	}	
	return(n - nleft);		/* return >= 0 */
}/* end readn */

/**
 * @brief 			ȷ��дn  ���ֽڵ���������
 */
ssize_t	writen(int fd, const void *vptr, size_t n){	
	size_t		nleft;	
	ssize_t		nwritten;	
	const char	*ptr;	
	ptr = vptr;	nleft = n;	
	while (nleft > 0) {		
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)	
				nwritten = 0;		/* and call write() again */			
			else				
				return(-1);			/* error */		
		}		
		nleft -= nwritten;		
		ptr   += nwritten;	
	}
	return(n);
}/* end writen */


/*
 * @brief	�Զ�����źų�ʱ�������
 			��������
 */
void signal_handler(union sigval v){
	int i;
	
	/*������δ�пͻ������ӹ�������ʱӦ���ж�maxi  ��ֵ*/
	if(maxi >= 0){		
		for(i = maxi; i >= 0; i--){
			if(client[i].socketfd < 0)
				continue;
			/*��ӻ��������������ݵ�һ���ԣ���ֹ��;�����������ط����ģ�
			   ���������Ч*/
			Pthread_mutex_lock(&client[i].socketfd_mt);			
			if(client[i].heart_beat_cnt >= HEART_MAX_CNT){
				/*������δ�ɹ�����������������*/
				printf("�ͻ�%s��ʱ��δ�ظ����ֶϿ�������...\n", client[i].client_id);
				LOGN("server", LOG_NOTICE,"heartbeat out %s", client[i].client_id);
				
				Close(client[i].socketfd);
				FD_CLR(client[i].socketfd, &allset);
				client[i].heart_beat_cnt = 0;
				client[i].socketfd = -1;
				client[i].socketfd_status = -1;
				bzero(client[i].client_id,CLIENT_ID_MAX);
				DestroyQueue(client[i].Q);		/*����ѭ�����У���ѭ������ֻ�����ӽ�����ʱ������ڴ�ռ�*/
				if(maxi == i){
					int k = i;
					maxi--;
					while(client[--k].socketfd < 0)
						maxi--;
				}
				Pthread_mutex_unlock(&client[i].socketfd_mt);
				continue;
				
				/*Ӧ�ùرտͻ�����ռ�õ���Դ*/
			}
			if(client[i].socketfd_status == -1){
				/*�����쳣����δ�յ�������*/

				LOGN("server", LOG_DEBUG, "client %s not recv the heartbeat+1", i, client[i].client_id);

				client[i].heart_beat_cnt++;
			}else{
				LOGN("server", LOG_DEBUG, "the client %s is working", client[i].client_id);
				/*�ͻ��˱��ִ��*/
				client[i].socketfd_status = -1;
				client[i].heart_beat_cnt = 0;
			}
			Pthread_mutex_unlock(&client[i].socketfd_mt);
			
			send_data(client[i].socketfd, COMM_TYPE_HEARBEAT, 0, NULL, 0);
			
			LOGN("server", LOG_DEBUG, "write  client %s MSG_HEART_BEAT", i,client[i].client_id);
		}
	}
	
}


void heart_beat_server(){
	LOGN("server", LOG_DEBUG, "start heart_beat...");

	set_timer(HEART_BEAT_TIME, 0, signal_handler, 100);		//������ʱ��
}

/*
 * @brief 	��������
 * @return	�ɹ����ط��͵��ֽ���������Ϊ0
 */
int send_data(int fd, int commtype, int comm, unsigned char *data, int len){
	int length;
	unsigned char *s_data;
	
	if(data == NULL)
		s_data = (unsigned char *)malloc(sizeof(unsigned char) * 9);
		
	else
		s_data = (unsigned char *)malloc(sizeof(unsigned char) * 2 *len);

	length = packet(s_data, (len == 0 ? NULL:data), len, commtype, comm);
	if(length > 0)
		writen(fd, s_data, length);

	free(s_data);
	return len;
}


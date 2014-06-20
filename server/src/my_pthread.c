#include "my_pthread.h"
#include "wrap.h"
#include "types.h"
#include "db_controller.h"
#include "socket.h"   	/*readn()*/
#include "wrap.h"		/*Malloc()*/
#include "logc.h"

#include <errno.h>
#include <unistd.h>		/*close(),*/
#include <string.h>		/*strncpy(), bzero()*/

extern fd_set allset;
extern int maxi,maxfd;

void read_client_id(int listenfd, char *buf, int len,int index){
	ssize_t nread;
	if((nread = readn(listenfd, buf, len)) < 0){		/*read error*/
		LOGN("server", LOG_ERROR, "read error,%s",strerror(errno));
		exit(-1);
	} 
#if DEBUG
	LOGN("server", LOG_DEBUG, "pthread %d read %s", index, buf);	
#endif
	return ;

}

/**
 * @brief 	���ǵĹ����߳���Ҫ������Ҫ����
 */
void *thread_main(void *arg){
	int connfd, listenfd, index;
	socklen_t clilen, addrlen;
	struct sockaddr cliaddr;
	char client_id[CLIENT_ID_MAX];

	index = LOWORD((int)arg);
	listenfd = HIWORD((int)arg);
	addrlen = sizeof(struct sockaddr);
#if DEBUG
	LOGN("server", LOG_DEBUG, "thread %d starting listening %d", index, listenfd);
#endif	
	for(; ;){
		clilen = addrlen;
		Pthread_mutex_lock(&mlock);
#if DEBUG		
		LOGN("server", LOG_DEBUG, "pthread %d accepting...", index);
#endif
again:
		if((connfd = accept(listenfd, &cliaddr, &clilen)) < 0){		/*accept  ����*/
#ifdef EPROTO   		/*Э�����*/
			if(errno == EPROTO || errno == ECONNABORTED || errno == EINTR)
#else 					/*���ӱ���ֹ*/
			if(errno == ECONNABORTED || errno == EINTR)
#endif
				goto again;
			else{
				LOGN("server", LOG_ERROR, "accept error");
				exit(-1);
			}	
		}
		
		Pthread_mutex_unlock(&mlock);

		/*�жϿͻ��ն˵�ID  �Ƿ�Ϸ�������Ϸ���connfd  ����TCP  ����*/
		read_client_id(connfd, client_id, CLIENT_ID_MAX, index);
		if(false == check_client_id(client_id)){		/*���ǺϷ�ID*/
			writen(connfd, "no", 3);
			Close(connfd);
		}else{											/*�Ϸ�ID*/
			int i;
			for(i = 0; i < CONN_NUM_MAX; i++){			/*��������������׽�������*/
				if(client[i].socketfd < 0){				/*�����ǳ��ν�����ӽ��������� �����ϲ��ú������ط�����*/
					client[i].socketfd = connfd;
					break;
				}
			}
			if(i == CONN_NUM_MAX){						/*�������������ܹ����ص���Ŀ�����߿ͻ����Ժ�����*/
				LOGN("server", LOG_DEBUG, "too many clients");
				client[i].socketfd = -1;
				Close(connfd);
				continue;
			}

			client[i].socketfd_status = 0;
			bzero(client[i].client_id, CLIENT_ID_MAX);
			strncpy(client[i].client_id, client_id, CLIENT_ID_MAX);
			FD_SET(connfd, &allset);					/*������ӽ�select�������׽���*/
			client[i].Q = Cqueue_init(CQUEUE_SIZE);		/*��ʼ��ѭ������*/
			writen(connfd, "ok", 3);
			
			if(connfd > maxfd)
				maxfd = connfd;
			if(i > maxi)
				maxi = i;
		}
	}
	
}

/**
 * @brief 			�������ǵĹ����̣߳����������׽���
 * @param index 		��ʾ�Ǽ����̳߳��еĵڼ����߳�
 * @param listenfd 	Ҫ�������׽���������
 */
int thread_make(int index, int listenfd){
	int n;

	if((n = pthread_create(&(tptr[index].thread_tid), NULL, &thread_main, (void *)(MAKEWORD(index, listenfd)))) == 0 ){
		return 0;
	}
	errno = n;				/*�����߳�ʧ��*/
#if DEBUG
	LOGN("server", LOG_DEBUG, "pthread_create %d error", index);
#endif
	return -1;			/*һ���̳߳��������߳̿��Լ������������Բ�ֱ���˳�*/
}


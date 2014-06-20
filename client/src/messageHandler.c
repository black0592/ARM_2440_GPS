#include "messageHandler.h"
#include "socket.h"
#include "logc.h"
#include "alarm.h"
#include "prot.h"
#include "fileo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int led2_status;

int get_message_from_lq(Cqueue *Q,unsigned char *desc);

void *messageHandler(void *param){

	/*ѭ���������*/
	while(1){
		/*��ʼ��ʱ�����û�еȺţ��������ͨ��������*/
			while(socket_fd_ctd.status != -1 && !QueueEmpty(socket_fd_ctd.Q)){				//���ӽ������Ҷ��в�Ϊ�յ������

				/*��������е���Ϣ*/
				unsigned char m_data[1024];
				int len;

				memset(m_data, 0x00, 1024);
				len = get_message_from_lq(socket_fd_ctd.Q, m_data);
				if(len == 0)
					continue;

				int comm_type = m_data[0];
				int comm ;
				switch(comm_type){
					case COMM_TYPE_HEARBEAT:
						socket_fd_ctd.socketfd_status = 0;
						break;
						
					case COMM_TYPE_SERVE:
						comm = m_data[1];
						
						if(comm == COMM_SERV_GPS){
							/*�ϴ�GPS ��Ϣ*/
							LOG(LOG_DEBUG, "�����Ҫ���ϴ�GPS������Ϣ");
#if IS_MINI2440
						}else if(comm == COMM_SERV_LOCK){
							/*����*/
							lock_car();
							LOG(LOG_DEBUG, "������´�����ָ�����...");
						}else if(comm == COMM_SERV_UNLOCK){
							/*����*/
							led2_status = 0;
							LOG(LOG_DEBUG, "������´����ָ��ѽ���...");
#endif							
						}else if(comm ==  COMM_SERV_BITMAP){
							/*�ϴ�ͼƬ*/
							static int count = 1;
							unsigned char file_name[6];
							snprintf((char *)file_name,6,"%d.jpg", count++);
							send_file((char *)file_name, socket_fd_ctd.socketfd);
							LOG(LOG_DEBUG, "�����Ҫ���ϴ�ͼƬ");
						}
						break;
					default:
						printf("�޷�ʶ���ָ��\n");
						break;
				}
			}
	}
}

/*
 * @brief	��ѭ��������ȡ��һ�����������ݰ�
 */
int get_message_from_lq(Cqueue *Q, unsigned char *desc){
	char temp;
	int count = 0;
	int len = 0, n;;
	
	while(Q->count && Q->base[Q->front] != HEAD){		//ȡ�ð�ͷ
		Q->front = (Q->front + 1) % Q->maxsize; 			//���ǰ�ͷ�Ļ��Ͷ���
		Q->count--;
	}
		
	count = 1;
	while(Q->count && (count + 1 <= Q->count) && (temp = Q->base[(Q->front + count)%Q->maxsize]) != END){				//ȡ�ð�β
		if(temp == HEAD){
			Q->front = (Q->front + count) % Q->maxsize;
			Q->count -= count;		
			count = 1;
			continue;
		}else
			count++;
	}
	if(count + 1 <= Q->count){		//���������������ݰ�����
		if((n = (Q->maxsize - Q->front)) >= (count + 1)){
			len = unpacket(desc,&Q->base[Q->front], count+1); //���Ҳ����������ݻص�ͷ��
		}
		else{
			unsigned char *temp_data;		/*���ѭ�������е�һ������*/
			temp_data = (unsigned char *)malloc(sizeof(unsigned char) * (count+1));
	
			memcpy(temp_data, Q->base + Q->front, n);
			memcpy(temp_data + n, Q->base, count + 1 - n);
			len = unpacket(desc, temp_data, count+1);
			free(temp_data);
		}
		PopupnQueue(Q ,count + 1);
	}
	
	return len;
}



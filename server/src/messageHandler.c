#include "messageHandler.h"
#include "socket.h"
#include "logc.h"
#include "prot.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


extern int maxi;

int get_message_from_lq(Cqueue *Q, unsigned char *desc);

void *messageHandler(void *param){
	int i;

	/*ѭ���������*/
	while(1){
		/*��ʼ��ʱ�����û�еȺţ��������ͨ������(û����������)*/
		for(i = 0; i <= maxi; i++){		/*ֻ���������ӵ�*/
			if(client[i].socketfd < 0)
				continue;
			while(!QueueEmpty(client[i].Q)){
				/*��������е���Ϣ*/
				unsigned char m_data[BUFFER_SIZE + NAME_MAX_SIZE + 4];
				int len;
				
				memset(m_data, 0x00, 1024);
				len = get_message_from_lq(client[i].Q, m_data);
				
				if(len == 0)
					continue;
				
				int comm_type = m_data[0];
				int comm ;

				switch(comm_type){
					case COMM_TYPE_ALARM:
						comm = m_data[1];

						if(comm == COMM_ALARM_BEYONGD_REGION){
							/*Խ�����򱨾�*/
							printf("�ͻ�%sԽ����ָ������!", client[i].client_id);
							LOGN("server", LOG_DEBUG,"�ͻ�%sԽ����ָ������!", client[i].client_id);
						}else if(comm == COMM_ALARM_CARJACKING){
							/*�ٳ�����*/
							printf("�ͻ�%s���ٳ���!", client[i].client_id);
							LOGN("server", LOG_DEBUG,"�ͻ�%s���ٳ���!", client[i].client_id);
						}else if(comm == COMM_ALARM_TIREPRESURE){
							/*̥ѹ����*/
							printf("�ͻ�%s̥ѹ������!", client[i].client_id);
							LOGN("server", LOG_DEBUG,"�ͻ�%s̥ѹ������!", client[i].client_id);
						}else if(comm == COMM_ALARM_BEYONGD_REGION_C){
							/*���Խ�����򱨾�*/
							printf("�ͻ�%s�ѷ���ָ������!", client[i].client_id);
							LOGN("server", LOG_DEBUG,"�ͻ�%s�ѷ���ָ������!", client[i].client_id);
						}else if(comm == COMM_ALARM_CARJACKING_C){
							/*����ٳ�����*/
							printf("�ͻ�%s����˽ٳ�����!", client[i].client_id);
							LOGN("server", LOG_DEBUG,"�ͻ�%s����˽ٳ�����!", client[i].client_id);
						}else if(comm == COMM_ALARM_TIREPRESURE_C){
							/*���̥ѹ����*/
							printf("�ͻ�%s̥ѹ�ѻָ�����!", client[i].client_id);
							LOGN("server", LOG_DEBUG,"�ͻ�%s̥ѹ�ѻָ�����!", client[i].client_id);
						}
						break;
					case COMM_TYPE_HEARBEAT:
						client[i].socketfd_status = 0;
						break;
					case COMM_TYPE_SERVE:
						comm = m_data[1];
						
						if(comm == COMM_SERV_GPS){
							//printf("�յ�GPS��\n");
							
						}else if(comm == COMM_SERV_BITMAP){
							/*�յ�ͼƬ*/
							//int length = *(int *)(m_data + 2);
							int fd;
							unsigned char *file_name;
							int file_name_len;

							file_name_len = *(int *)(m_data + FILE_NAME_LEN_START);
							file_name = (unsigned char *)malloc(sizeof(unsigned char) * file_name_len);
							memcpy(file_name, m_data + FILE_NAME_START, file_name_len);
							fd = open((char *)file_name, O_CREAT | O_RDWR | O_APPEND, 0777);
							if(fd < 0){
								perror("can't open 1.jpg");
								continue;
							}
								
							writen(fd, m_data + FILE_NAME_START + file_name_len, len - (FILE_NAME_START + file_name_len ));
							free(file_name);
							close(fd);
						}
						break;
					default:
						printf("��֪������ʲô����\n");
						break;
				}
			}
		}
	}
}

/*
 * @brief	��ѭ��������ȡ��һ�����������ݰ�
 * @return	ȡ�������ݵĳ���(���֮��)
 */
int get_message_from_lq(Cqueue *Q, unsigned char *desc){
	char temp;
	int count = 0;
	int len = 0, n;;

	while(Q->count && Q->base[Q->front] != HEAD){ 		//ȡ�ð�ͷ
		Q->front = (Q->front + 1) % Q->maxsize;				//���ǰ�ͷ�Ļ��Ͷ���
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
	if(count + 1 <= Q->count){		//���������������ݰ�����,��ȡ�����ݴ���
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


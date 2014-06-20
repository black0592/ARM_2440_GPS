#include "menu.h"
#include "socket.h"
#include "config.h"
#include "my_io.h"

#include<stdlib.h>
#include<ctype.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern int maxi;

int input_client_id(char *client_id);


void eatline(void){
	while(getchar() != '\n');
}

MENU_LIST* pop(MENU_LIST *top){
	top = top->next;
	return top;
}

MENU_LIST* push(MENU_LIST *top,MENU_LIST *p){
	p->next = top;
	top = p;
	return top;
}



void show_menu(MENU_LIST *menu_show);

void *showmenu(void *param){
	MENU_LIST *p = NULL,*top = NULL;
	int choice;
	MENU *select_menu = NULL;
	
	while(1){
		if(!top || select_menu){
			if((p = malloc(sizeof(MENU_LIST))) == NULL){
				perror("�����ڴ�ʧ�ܣ�");
				exit(1);
			}
			if(!top){					
				strcpy(p->title,"���˵�");
				p->menu = main_menu;
				p->num = 5;
			}
			else{
				strcpy(p->title,select_menu->title);
				p->menu = select_menu->cmenu;
				p->num = select_menu->cmenu_num;
			}
			top = push(top,p);	
		}
		
		select_menu = NULL;
		choice = 0;
		
		show_menu(top);
		printf("��ѡ��˵���");
		scanf("%d",&choice);
		eatline();
		if(choice < 0 || choice > top->num){
			printf("������Ч,������ѡ��\n");
			continue;
		}
		if(choice == top->num){
			p = top;
			top = pop(top);
			free(p);
			if(!top)
				break;
			else{
				continue;
			}
		}

		select_menu = &top->menu[choice - 1];

		//�д�������
		if(select_menu->proc){
			//(*select_menu->proc)(select_menu->param);
			char client_id[CLIENT_ID_MAX];
			int index;
			
			printf("��ѡ��ͻ�(���ƺ�): ");
get_again:
	
			//fgets(client_id, CLIENT_ID_MAX, stdin);
			while(!get_in(client_id, CLIENT_ID_MAX-1));
			while(index <= maxi){
				if(strncmp(client[index].client_id, client_id, CLIENT_ID_MAX - 1) == 0)  //ȥ���������ͻ��з�
					break;
				index++;
			}

			if(index > maxi){
				printf("�ͻ������ڻ���δ����,����������: ");
				goto get_again;
			}
			if(strcmp(select_menu->title,"��λ����") == 0){
				send_data(client[index].socketfd, COMM_TYPE_SERVE, COMM_SERV_GPS, NULL, 0);
			}
			else if(strcmp(select_menu->title,"���ӳ����ڲ����") == 0){
				send_data(client[index].socketfd, COMM_TYPE_SERVE, COMM_SERV_BITMAP, NULL, 0);
			}
			else if(strcmp(select_menu->title,"�´�����ָ��") == 0){
				send_data(client[index].socketfd, COMM_TYPE_SERVE, COMM_SERV_LOCK, NULL, 0);
			}
			else if(strcmp(select_menu->title,"��������") == 0){
				send_data(client[index].socketfd, COMM_TYPE_SERVE, COMM_SERV_UNLOCK, NULL, 0);
			}
			select_menu = NULL;
		}else if(!select_menu->cmenu){     //�޴������������Ӳ˵���
			select_menu = NULL;
			printf("������Ч,������ѡ��\n");
		}
	}
	exit(0);
}

void show_menu(MENU_LIST *menu_show){
	int i = 0;
	printf("\n    %s	\n",menu_show->title);
	for(i = 0; i < 15; i++)
		printf("-");
	printf("\n");
	for(i = 0; i < menu_show->num; i++)
		printf("[%d].%s\n",i+1,menu_show->menu[i].title);	
}

int input_client_id(char *client_id){
	int len = 0;
	int ch;
	while((ch = getch()) != '\r'){       //���˵�������
		if(ch == 8 && len > 0){
			printf("\b \b");               //����VBKeyBackʱ��ʵ��ɾ��Ч��
			len--;
		}
		else if(isalnum(ch)){
			if(isalpha(ch) || len >= (CLIENT_ID_MAX-1)){		       //��Ӣ����ĸʱ������ʾ��
				printf("\a");
			}           
			else{                         //������ʱ�Ż���
				client_id[len++] = ch;
				printf("%c",ch);
			}
		}
	}
	printf("\n");
	if(len != (CLIENT_ID_MAX-1)){                 //�������󷵻�0
		if(len == 0)
			puts("���󣡳��ƺŲ���Ϊ�գ�����������");
		else
			printf("���󣡳��ƺ���Ϊ%dλ������������\n",11);
		return 0;
	}
	client_id[len] = '\0';
	return 1;
}




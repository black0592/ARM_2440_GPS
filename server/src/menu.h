#ifndef __MENU_H__
#define __MENU_H__

#include "types.h"

#define MENU_TITLE_MAX_LEN 20


typedef struct menu_s MENU;
struct menu_s{
	char title[MENU_TITLE_MAX_LEN];     /*�˵�������Ϣ*/
	const MENU *cmenu;									/*�Ӳ˵�*/
	int cmenu_num;											/*�Ӳ˵�ѡ�����*/
	int proc;   												/*�Ƿ��д������*/
};

typedef struct menu_list MENU_LIST;
struct menu_list{
	char title[MENU_TITLE_MAX_LEN];
	MENU *menu;
	int num;
	MENU_LIST *next;
};

//��߲�����static����
static const MENU main_menu[] = {
	{"��λ����", NULL, 0, 1},						/*�ϴ�GPS*/
	{"���ӳ����ڲ����", NULL, 0, 1},		/*�ϴ�ͼƬ*/
	{"�´�����ָ��", NULL, 0, 1},				/*����*/
	{"��������", NULL, 0, 1},						/*����*/
	{"�˳�", NULL, 0, 0}
};

extern void *showmenu(void *param);

#endif	/*__MENU_H__*/

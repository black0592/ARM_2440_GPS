#ifndef __CIRCLEQUEUE_H__		/*��ѭ�������й�*/
#define __CIRCLEQUEUE_H__

typedef struct {
	volatile int front;				/*����ͷ�α�*/
	volatile int rear;				/*����β�α�*/
	int maxsize;			/*���еĳ���*/
	volatile int count;				/*ָʾ��ǰ����֮�е�Ԫ�ظ���*/
	unsigned char *base;	/*���е�ͷָ��*/
}Cqueue;

Cqueue* Cqueue_init(int size);
int QueueEmpty(Cqueue *Q);
int QueueFull(Cqueue *Q, int n);
int EnterQueue(Cqueue *Q, unsigned char *items, int  n;);
unsigned char  PopupQueue(Cqueue *Q);
int PopupnQueue(Cqueue *Q,int n);
void DestroyQueue(Cqueue *Q);

#endif /* __CIRCLEQUEUE_H__ */

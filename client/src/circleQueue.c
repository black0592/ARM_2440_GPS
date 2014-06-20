#include "circleQueue.h"
#include "wrap.h"
#include "logc.h"

#include <string.h>			/*memcpy*/

Cqueue* Cqueue_init(int size){
	Cqueue *Q = (Cqueue *) malloc(sizeof(Cqueue));
	Q->base = Malloc(size * sizeof(unsigned char));
	Q->front = 0;
	Q->rear = 0;
	Q->count = 0;
	Q->maxsize = size;
	return Q;
}

int QueueEmpty(Cqueue *Q){
	return ((Q->count == 0)?1:0);	
}

/**
 * @brief		�ж�ѭ�������Ƿ񹻴洢Ҫ���������
 * @param 	n	Ҫ��������ݴ�С
 */
int QueueFull(Cqueue *Q,int n){
	return ((Q->maxsize < (Q->count + n)) ? 1 : 0);	
}

/**
 * @brief 			�����ݴ���ѭ��������
 * @param 	Q		ѭ������ָ��
 * @param 	items	Ҫ���������ָ��
 * @param 	n		��������ݳ���
 */
int EnterQueue(Cqueue *Q, unsigned char *items, int n){
	if(QueueFull(Q, n)){
		return 0;
	}	
	int len;
	
	if((len = Q->maxsize - Q->rear) >= n)
		memcpy(Q->base + Q->rear, items, n);	/*֮ǰ���û�жϻ�Խ��*/
	else{
		memcpy(Q->base + Q->rear, items, len);
		memcpy(Q->base, items+len, n - len);
	}
	Q->rear = (Q->rear + n) % Q->maxsize;
	Q->count += n;

	return n;
}


unsigned char  PopupQueue(Cqueue *Q){
	unsigned char temp;
	if(QueueEmpty(Q)){
		printf("queue is empty\n");
		exit(-1);	
	}	
	temp = Q->base[Q->front];
	Q->front = (Q->front + 1)%Q->maxsize;
	Q->count--;
	
	return temp;
}
/**
 * @brief	�Ӷ�����ȡ��n  ������
 * @param 	Q		ѭ������
 * @param 	n		Ҫȡ�������ݳ���
 */
int PopupnQueue(Cqueue *Q,int n){
	if(n > Q->count){
		printf("��ǰ����������С��n\n");
		exit(-1);
	}

	Q->front = (Q->front + n)%Q->maxsize;
	Q->count -= n;

	return n;
}


void DestroyQueue(Cqueue *Q){
	if(Q->base != NULL)
		free(Q->base);
	Q->base = NULL;
	if(Q != NULL)
		free(Q);
	Q = NULL;
}

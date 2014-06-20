#ifndef __TYPES_H__
#define __TYPES_H__

/*��������*/
#ifndef BOOL
typedef int BOOL;
#endif
#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef bool
#define bool BOOL
#endif
#ifndef false
#define false (0)
#endif
#ifndef true
#define true (!false)
#endif

#ifndef NULL
#define NULL (0)
#endif
#ifndef null
#define null (0)
#endif

/*WORD������*/
#define MAKEWORD(x, y) (int)(((int)(x) & 0xffff) | ((((int)(y)) & 0xffff)<< 16 ))   /*x  Ϊ��2  ���ֽ�*/
#define LOWORD(w) (int)(((int)(w)) & 0xffff)
#define HIWORD(w) (int)((((int)(w)) & 0xffff0000) >> 16)


#endif /* __TYPES_H__ */

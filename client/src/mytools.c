#include "mytools.h"
#include "logc.h"

//#include <sys/time.h> /*setitimer*/
#include <string.h>  /*memset*/
#include <stdio.h>	/*perror*/
#include <stdlib.h>	/*exit*/
#include <errno.h>

/*
 * @brief 	�����źŴ��õķ���������sigacionʵ�֣���ϵͳ�ĸ���
 */
Sigfunc *signal(int signo, Sigfunc *func){
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(signo == SIGALRM){
#ifdef 	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	}else{
#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	if(sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}

/**
 * @brief 			������ʱ��
 * @param sec 		��
 * @param usec		΢��
 * @param func		��ʱ��������
 * @param sigval_int	��ʱ����ʾ��
 */
void set_timer(long sec, long usec, Timerfunc *func, int sigval_int){
	timer_t timerid;
	struct sigevent evp;
	memset(&evp, 0, sizeof(struct sigevent));		//�����ʼ��

	evp.sigev_value.sival_int = sigval_int;
	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = func;

	if (timer_create(CLOCK_REALTIME, &evp, &timerid) == -1)
	{
		LOG(LOG_ERROR, "fail to timer_create:%s", strerror(errno));
		exit(-1);
	}

	struct itimerspec it;
	it.it_interval.tv_sec = sec;
	it.it_interval.tv_nsec = usec;
	it.it_value.tv_sec = sec;
	it.it_value.tv_nsec = usec;

	if (timer_settime(timerid, 0, &it, NULL) == -1)
	{
		LOG(LOG_ERROR, "fail to timer_settime:%s", strerror(errno));
		exit(-1);
	}
}

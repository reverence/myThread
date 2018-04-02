/*
 * schedule.c
 *
 *  Created on: Mar 29, 2018
 *      Author: chengzhang
 */

#include "thread.h"
#include <sys/time.h>
#include <signal.h>

void context_switch();//汇编

extern my_task* tasks[MAX_TASKS];
extern my_task* current_task;

int count_array[MAX_TASKS] = {-1};

int get_current_time()
{
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000+t.tv_usec/1000;
}

my_task* getNext()
{
	int i;
	repeat:
	for(i =0;i < MAX_TASKS;i++)
	{
		if(tasks[i] && tasks[i]->status == SLEEPING)
		{//如果有sleep时间到的，唤醒，重新争用cpu
			if(get_current_time() > tasks[i]->wake_up_time)
			{
				tasks[i]->status = RUNNABLE;
				tasks[i]->time_slice = DEFAULT_TIME_SLICE;//恢复时间片
			}
		}
	}
	//如果线程时间片用光了，这里恢复,有个问题，这里应该没有可调度的线程时才能恢复,不能直接找出时间片为0的直接恢复
	//这样找，只要有time_slice不为0，就应该继续调度
	//获取有效的可调度线程
	memset(count_array,-1,MAX_TASKS);
	int c = 0;
	for(i=0;i < MAX_TASKS;i++)
	{
		if(tasks[i] && tasks[i]->status == RUNNABLE && tasks[i]->time_slice>0)
		{
			count_array[c] = i;
			c++;
		}
	}
	//如果还是没有可调度的，恢复时间片，继续
	if(!c)
	{
		for(i=0;i<MAX_TASKS;i++)
		{
			if(tasks[i] && tasks[i]->status != EXITED)
			{
				tasks[i]->time_slice = DEFAULT_TIME_SLICE;
			}
		}
		goto repeat;
	}

	//随机选出一个来调度
	srand((int)time(NULL));
	i = rand()%c;
	int index = count_array[i];
	return tasks[index];
}

void my_schedule()
{
	//关中断，保证正确切换
	cli();
	my_task* task = getNext();
	if(task)
	{
		context_switch(task);
	}

}

void sleep(int seconds)
{
	current_task->wake_up_time = get_current_time()+seconds*1000;
	current_task->status = SLEEPING;
	current_task->time_slice = DEFAULT_TIME_SLICE;
	my_schedule();
}

void yield()
{
	current_task->status = RUNNABLE;
	current_task->time_slice = DEFAULT_TIME_SLICE;
	my_schedule();
}

void join(int tid)
{
	current_task->status = RUNNABLE;
	while(tasks[tid]->status!=EXITED)
	{
		current_task->time_slice = DEFAULT_TIME_SLICE;
		my_schedule();
	}
	free(tasks[tid]);
	tasks[tid] = NULL;
}

void cli() {//windows下有这个指令关中断，linux下不知道是什么
    sigset_t t;
    sigemptyset(&t);
    sigaddset(&t, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &t, NULL) < 0) {
      printf("sigprocmask error\n");
      exit(1);
    }
}

void sti() {//windows下有这个指令开中断，linux下不知道是什么
    sigset_t t;
    sigemptyset(&t);
    sigaddset(&t, SIGALRM);
    if (sigprocmask(SIG_UNBLOCK, &t, NULL) < 0) {
    	 printf("sigprocmask error\n");
    	 exit(1);
    }
}

void timer_worker()
{
	if(--current_task->time_slice > 0)
	{
		return;
	}
	current_task->status = RUNNABLE;
	my_schedule();
}

void init_timer()
{
	signal(SIGALRM, timer_worker);
	struct itimerval val,old_value;
	val.it_value.tv_sec = 0;
	val.it_value.tv_usec = 1000;
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 1*1000; // 10 ms
	if (setitimer(ITIMER_REAL, &val, &old_value) < 0)
	{
		printf("set timer failed\n");
		exit(1);
	}
}

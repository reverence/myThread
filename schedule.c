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

int get_current_time()
{
	struct timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec*1000+t.tv_usec/1000;
}

my_task* getNext()
{
	int i;
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
	int flag = 0;
	for(i=0; i<MAX_TASKS;i++)
	{
		if(tasks[i] && tasks[i]->status != EXITED)
		{
			if(tasks[i]->time_slice > 0 && tasks[i]->status==RUNNABLE)
			{
				flag = 1;
				break;
			}
		}
	}
	if(!flag)
	{
		for(i=0; i<MAX_TASKS;i++)
		{
			if(tasks[i] && tasks[i]->status == RUNNABLE)
			{
				tasks[i]->time_slice = DEFAULT_TIME_SLICE;
			}
		}
	}

	my_task* next = NULL;
	while(1)
	{
		i = rand()%MAX_TASKS;//随机选取一个
		if(tasks[i] && tasks[i]->status == RUNNABLE && tasks[i]->time_slice>0)
		{
			next = tasks[i];
			break;
		}
	}
	return next;
}

void my_schedule()
{
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
	my_schedule();
}

void yield()
{
	current_task->status = RUNNABLE;
	my_schedule();
}

void join(int tid)
{
	current_task->status = RUNNABLE;
	while(tasks[tid]->status!=EXITED)
	{
		my_schedule();
	}
	free(tasks[tid]);
	tasks[tid] = NULL;
}


void timer_worker()
{
	if(--current_task->time_slice > 0)
	{
		return;
	}
	current_task->time_slice = 0;
	my_schedule();
}

void init_timer()
{
	struct itimerval val;
	val.it_value.tv_sec = 0;
	val.it_value.tv_usec = 1000;
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 1000*10; // 10 ms
	if (setitimer(ITIMER_REAL, &val, NULL) < 0)
	{
		printf("set timer failed\n");
		exit(1);
	}
  signal(SIGALRM, timer_worker);
}

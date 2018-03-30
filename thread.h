/*
 * thread.h
 *
 *  Created on: Mar 27, 2018
 *      Author: chengzhang
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <stdlib.h>

/**************默认栈大小****************/
#define STACK_SIZE 1024
/**************默认支持的最大任务数****************/
#define MAX_TASKS 16
/**************默认时间片大小****************/
#define DEFAULT_TIME_SLICE 5

/*******************线程状态*********************/
#define INITIALIZED 0
#define RUNNABLE 1
#define RUNNING	2
#define	SLEEPING 3
#define EXITED 4


typedef struct my_task{
	int id;//任务标识
	void (*fun)();//指向线程执行的函数地址
	int esp;//栈顶指针，保存当前线程的栈
	int stack[STACK_SIZE];//线程私有的栈，默认大小1024
	int status;//线程当前状态
	unsigned int wake_up_time;//唤醒时间
	int time_slice;//时间片
}my_task;

//////////////////////////////////////////////////////
void my_schedule();//真正处理线程的切换

/********************function**********************/
int thread_create(int *tid, void (*start)());

void sleep(int seconds);

void yield();

void join(int tid);

#endif /* THREAD_H_ */

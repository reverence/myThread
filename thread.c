/*
 * thread.c
 *
 *  Created on: Mar 29, 2018
 *      Author: chengzhang
 */

#include "thread.h"

////////////////////////////////////////////////////
/**
 * 考虑到创建线程时，主线程正在运行，主线程切换时也要保存栈信息，所以tasks中应该初始化放一个主线程对象占个坑
 */
my_task main_task = {0,NULL,0,{0},RUNNING,0};

my_task* current_task = &main_task;

my_task *tasks[MAX_TASKS] = {&main_task,};

void uni_enter(my_task* task)
{
	task->status = RUNNING;
	task->fun();//执行线程对应的函数
	task->status = EXITED;//线程执行结束后，更改状态为退出
	my_schedule();//继续切换到另一个线程执行
}


/**
 * 创建线程
 */
int thread_create(int *tid, void (*start)()){

	if(!tid || !start){
		printf("parameters invalid\n");
		exit(1);
	}

	my_task* task = (my_task*)malloc(sizeof(my_task));
	if(!task){
		printf("create thread failed\n");
		exit(1);
	}

	int id = 0;
	while(id < MAX_TASKS){
		if(!tasks[id]){
			break;
		}
		id++;
	}
	if(id >= MAX_TASKS){
		printf("create thread failed\n");
		exit(1);
	}

	*tid = id;

	task->id = id;
	task->fun = start;
	task->status = INITIALIZED;
	task->wake_up_time = 0;
	task->time_slice = DEFAULT_TIME_SLICE;//默认50ms
	int* stack = task->stack;
	task->esp = (int)(stack+STACK_SIZE-11);
	//栈向下生长
	//这里为了push操作，pop操作能够对应得上，我们保留了当前线程的寄存器状态elfags--eax,实际上在这里没有啥用处
	stack[STACK_SIZE-11] = 7; // eflags
	stack[STACK_SIZE-10] = 6; // edi
	stack[STACK_SIZE-9] = 5; // esi
	stack[STACK_SIZE-8] = 4; // edx
	stack[STACK_SIZE-7] = 3; // ecx
	stack[STACK_SIZE-6] = 2; // ebx
	stack[STACK_SIZE-5] = 1; // eax
	stack[STACK_SIZE-4] = 0; // old ebp
	stack[STACK_SIZE-3] = (int)uni_enter; //如果当前线程让出cpu后，我们是不知道该切换到哪个线程的栈上去执行，所以使用了一个统一的enter入口来决定线程的执行

	stack[STACK_SIZE-2] = 8;//这里是一个未知的返回地址(为了ret指令返回地址用)
	stack[STACK_SIZE-1] = (int)task;//enter函数的参数

	task->status = RUNNABLE;
	tasks[id] = task;

	return 0;
}



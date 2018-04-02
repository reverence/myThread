/*
 * main.c
 *
 *  Created on: Mar 28, 2018
 *      Author: chengzhang
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char s[32] = {'\0'};

extern void init_timer();

char* get_time()
{
	time_t tt = time(0);
	memset(s,'\0',32);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", localtime(&tt));
	return s;
}

/**
 * 用于线程1执行
 */
int fun1(){
	int i =3;
	while(i--){
		printf("%s\tfun1 is running at %d\n",get_time(),i);
		if(i==1){
			sleep(5);
		}
	}
	printf("%s\tfun1 exit\n",get_time());
}

/**
 * 用于线程2执行
 */
int fun2(){
	int i =3;
	while(i--){
		printf("%s\tfun2 is running at %d\n",get_time(),i);
		if(i==2){
			sleep(10);
		}
	}
	printf("%s\tfun2 exit\n",get_time());
}

/**
 * 用于线程3执行
 */
int fun3(){
	int i =3;
	while(i--){
		printf("%s\tfun3 is running at %d\n",get_time(),i);
		if(i==1){
			yield();
		}
	}
	printf("%s\tfun3 exit\n",get_time());
}

/**
 * 用于线程4执行
 */
int fun4(){
	int i =3;
	while(i--){
		printf("%s\tfun4 is running at %d\n",get_time(),i);
		if(i==2){
			yield();
		}
	}
	printf("%s\tfun4 exit\n",get_time());
}

/**
 * 测试一直循环
 */
int fun5()
{
	int i = 5;
	int j,k;
	while(i--)
	{
		printf("%s\tfun5 is running at %d\n",get_time(),i);
		for(j=0;j<10000;j++)
		{
			for(k=0;k<10000;k++)
			{

			}
		}
		printf("%s\tfun5 is end at %d\n",get_time(),i);
	}
	printf("%s\tfun5 exit\n",get_time());
}

int main(){

	init_timer();
	printf("begin main\n");
	int tid1,tid2,tid3,tid4;
	thread_create(&tid1,fun1);
	printf("create thread %d for exec fun1\n",tid1);
	thread_create(&tid2,fun2);
	printf("create thread %d for exec fun2\n",tid2);
	thread_create(&tid3,fun3);
	printf("create thread %d for exec fun3\n",tid3);
	thread_create(&tid4,fun4);
	printf("create thread %d for exec fun4\n",tid4);

	int tid5;
	thread_create(&tid5,fun5);
	printf("create thread %d for exec fun5\n",tid5);

	join(tid1);//等待线程1执行完
	join(tid2);//等待线程2执行完
	join(tid3);//等待线程3执行完
	join(tid4);//等待线程4执行完
	join(tid5);//等待线程5执行完

	printf("main exited\n");

	return 0;
}

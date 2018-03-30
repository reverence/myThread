all:main
main:main.c thread.c thread.h schedule.c my_switch.s
	gcc -m32 -g main.c thread.c schedule.c my_switch.s -o main

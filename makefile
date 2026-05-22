main : main.c routine.s scheduler.c
	cc -o main -I. main.c scheduler.c routine.s -g

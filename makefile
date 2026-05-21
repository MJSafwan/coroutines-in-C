main : main.c routine.s
	cc -o main -I. main.c routine.s -g

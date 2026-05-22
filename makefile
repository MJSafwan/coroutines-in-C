main : example.c routine.s scheduler.c
	cc -o example -I. example.c scheduler.c routine.s -g

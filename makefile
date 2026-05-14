main : main.c longjmp.s routine.s
	clang -o main main.c longjmp.s routine.s -g -O0

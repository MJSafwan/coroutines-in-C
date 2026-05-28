ASYNC_DIR = ../..

main: main.c $(ASYNC_DIR)/routine.s $(ASYNC_DIR)/scheduler.c
	cc -o main -I$(ASYNC_DIR) $^ -g

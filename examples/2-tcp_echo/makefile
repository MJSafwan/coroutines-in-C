ASYNC_DIR = ../..

main : main.c $(ASYNC_DIR)/scheduler.c $(ASYNC_DIR)/routine.s
	clang -o main -I$(ASYNC_DIR) $^ -g

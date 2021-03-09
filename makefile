#Sample Makefile for Malloc
CC=g++
CFLAGS= --std=c++11 -g -O0 -Wall -lpthread
Target = test_scheduler
test_scheduler:
	$(CC) $(CFLAGS)  utils.h utils.cc mutex.h mutex.cc thread.h thread.cc fiber.h fiber.cc scheduler.h scheduler.cc macro.h test_scheduler.cc -o $(Target)

# test_fiber:
# 	$(CC) $(CFLAGS) test_fiber.cc fiber.h fiber.cc macro.h utils.h utils.cc -o fiber_test

# test1: 
# 	$(CC) $(CFLAGS) test.cc macro.h utils.cc utils.h -pthread -o test

.PHONY:
clean:
	rm -rf  *.o *.gch test_scheduler
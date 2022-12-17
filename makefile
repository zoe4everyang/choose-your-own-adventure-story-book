CFLAGS = -Wall -Werror -std=gnu++98
PROGS = cyoa-step1 cyoa-step2 cyoa-step3 cyoa-step4
OBJS=$(patsubst %,%.o,$(PROGS)) 
all: $(PROGS)
cyoa-step%:cyoa-step%.o cyoa.o
	g++ -o $@ $^
cyoa-step%.o: cyoa-step%.cpp
	g++ $(CFLAGS) -c $<
cyoa.o: cyoa.h

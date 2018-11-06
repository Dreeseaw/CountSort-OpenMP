CC=g++
CFLAGS= -O3 -fopenmp -mavx -std=c++11
NAME= sorting

.PHONY: all
all:
	$(CC) $(CFLAGS) -o $(NAME) $(NAME).cpp

.PHONY: clean
clean:
	rm -f $(NAME) $(NAME).o thr.pbs.o*

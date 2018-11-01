CC = g++
OBJS = h1.o h2.o join.o main.o structs.o
FLAGS = -g3 -Wall

all:$(OBJS)
	@echo Compiling...
	@$(CC) -o join $(OBJS) $(FLAGS)


h1.o:h1.cpp
	$(CC) -c h1.cpp $(FLAGS)

h2.o:h2.cpp
	$(CC) -c h2.cpp $(FLAGS)

join.o:h1.cpp
	$(CC) -c join.cpp $(FLAGS)

main.o:h1.cpp
	$(CC) -c main.cpp $(FLAGS)

structs.o:h1.cpp
	$(CC) -c structs.cpp $(FLAGS)

clean:
	rm -rf *.o a.out

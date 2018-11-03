CC = g++
OBJS = h1.o h2.o join.o main.o structs.o result.o
FLAGS = -g3 -Wall

all:$(OBJS)
	$(CC) -o join $(OBJS) $(FLAGS)

h1.o:h1.cpp
	$(CC) -c h1.cpp $(FLAGS)

h2.o:h2.cpp
	$(CC) -c h2.cpp $(FLAGS)

join.o:join.cpp
	$(CC) -c join.cpp $(FLAGS)

result.o:result.cpp
	$(CC) -c result.cpp $(FLAGS)

main.o:main.cpp
	$(CC) -c main.cpp $(FLAGS)

structs.o:structs.cpp
	$(CC) -c structs.cpp $(FLAGS)

clean:
	rm -rf *.o a.out join *.gch

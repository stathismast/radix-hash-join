CC = g++
OBJS = ./exec/h1.o ./exec/h2.o ./exec/join.o ./exec/main.o \
		./exec/structs.o ./exec/result.o
FLAGS = -g3 -Wall

all:$(OBJS)
	$(CC) -o ./exec/joinTest $(OBJS) $(FLAGS)

./exec/h1.o:./join/hashFunctions/h1.cpp
	$(CC) -c ./join/hashFunctions/h1.cpp $(FLAGS) -o ./exec/h1.o

./exec/h2.o:./join/hashFunctions/h2.cpp
	$(CC) -c ./join/hashFunctions/h2.cpp $(FLAGS) -o ./exec/h2.o

./exec/join.o:./join/join.cpp
	$(CC) -c ./join/join.cpp $(FLAGS) -o ./exec/join.o

./exec/result.o:./join/dataStructures/result.cpp
	$(CC) -c ./join/dataStructures/result.cpp $(FLAGS) -o ./exec/result.o

./exec/main.o:main.cpp
	$(CC) -c main.cpp $(FLAGS) -o ./exec/main.o

./exec/structs.o:./join/dataStructures/structs.cpp
	$(CC) -c ./join/dataStructures/structs.cpp $(FLAGS) -o ./exec/structs.o

clean:
	rm -rf ./exec/*.o a.out ./exec/joinTest *.gch

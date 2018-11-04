CC = g++
OBJS = ./join/hashFunctions/h1.o ./join/hashFunctions/h2.o ./join/join.o main.o \
		./join/dataStructures/structs.o ./join/dataStructures/result.o
FLAGS = -g3 -Wall

all:$(OBJS)
	$(CC) -o joinTest $(OBJS) $(FLAGS)

./join/hashFunctions/h1.o:./join/hashFunctions/h1.cpp
	$(CC) -c ./join/hashFunctions/h1.cpp $(FLAGS) -o ./join/hashFunctions/h1.o

./join/hashFunctions/h2.o:./join/hashFunctions/h2.cpp
	$(CC) -c ./join/hashFunctions/h2.cpp $(FLAGS) -o ./join/hashFunctions/h2.o

./join/join.o:./join/join.cpp
	$(CC) -c ./join/join.cpp $(FLAGS) -o ./join/join.o

./join/dataStructures/result.o:./join/dataStructures/result.cpp
	$(CC) -c ./join/dataStructures/result.cpp $(FLAGS) -o ./join/dataStructures/result.o

main.o:main.cpp
	$(CC) -c main.cpp $(FLAGS) -o main.o

./join/dataStructures/structs.o:./join/dataStructures/structs.cpp
	$(CC) -c ./join/dataStructures/structs.cpp $(FLAGS) -o ./join/dataStructures/structs.o

clean:
	rm -rf ./*/*.o *.o ./*/*/*.o a.out joinTest ./*/*.gch *.gch ./*/*/*.gch

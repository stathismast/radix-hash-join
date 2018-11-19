CC = g++
OBJS = ./join/h1.o ./join/h2.o ./join/join.o main.o \
		./join/structs.o ./join/result.o
SERIAL_OBJS = ./join/h1.o ./join/h2.o ./join/join.o \
		./join/structs.o ./join/result.o \
		./testMain/serialJoin.o
ODD_EVEN_OBJS = ./join/h1.o ./join/h2.o ./join/join.o \
		./join/structs.o ./join/result.o \
		./testMain/oddEvenJoin.o
RANDOM_OBJS = ./join/h1.o ./join/h2.o ./join/join.o \
		./join/structs.o ./join/result.o \
		./testMain/randomJoin.o
FLAGS = -g3 -Wall

all:$(OBJS)
	$(CC) -o main $(OBJS) $(FLAGS)

./join/h1.o:./join/h1.cpp
	$(CC) -c ./join/h1.cpp $(FLAGS) -o ./join/h1.o

./join/h2.o:./join/h2.cpp
	$(CC) -c ./join/h2.cpp $(FLAGS) -o ./join/h2.o

./join/join.o:./join/join.cpp
	$(CC) -c ./join/join.cpp $(FLAGS) -o ./join/join.o

./join/result.o:./join/result.cpp
	$(CC) -c ./join/result.cpp $(FLAGS) -o ./join/result.o

main.o:main.cpp
	$(CC) -c main.cpp $(FLAGS) -o main.o

./join/structs.o:./join/structs.cpp
	$(CC) -c ./join/structs.cpp $(FLAGS) -o ./join/structs.o

serialJoin:$(SERIAL_OBJS)
	$(CC) -o serialJoin $(SERIAL_OBJS) $(FLAGS)

./testMain/serialJoin.o:./testMain/serialJoin.cpp
	$(CC) -c ./testMain/serialJoin.cpp $(FLAGS) -o ./testMain/serialJoin.o

oddEvenJoin:$(ODD_EVEN_OBJS)
	$(CC) -o oddEvenJoin $(ODD_EVEN_OBJS) $(FLAGS)

./testMain/oddEvenJoin.o:./testMain/serialJoin.cpp
	$(CC) -c ./testMain/oddEvenJoin.cpp $(FLAGS) -o ./testMain/oddEvenJoin.o

randomJoin:$(RANDOM_OBJS)
	$(CC) -o randomJoin $(RANDOM_OBJS) $(FLAGS)

./testMain/randomJoin.o:./testMain/randomJoin.cpp
	$(CC) -c ./testMain/randomJoin.cpp $(FLAGS) -o ./testMain/randomJoin.o

clean:
	rm -rf ./*/*.o *.o ./*/*/*.o a.out main randomJoin serialJoin \
		oddEvenJoin ./*/*.gch *.gch ./*/*/*.gch

CC = g++
OBJS = main.o ./memmap/memmap.o
SERIAL_OBJS = ./singleJoin/h1.o ./singleJoin/h2.o ./singleJoin/join.o \
		./singleJoin/structs.o ./singleJoin/result.o \
		./testMain/serialJoin.o
ODD_EVEN_OBJS = ./singleJoin/h1.o ./singleJoin/h2.o ./singleJoin/join.o \
		./singleJoin/structs.o ./singleJoin/result.o \
		./testMain/oddEvenJoin.o
RANDOM_OBJS = ./singleJoin/h1.o ./singleJoin/h2.o ./singleJoin/join.o \
		./singleJoin/structs.o ./singleJoin/result.o \
		./testMain/randomJoin.o
FLAGS = -g3 -Wall

all:$(OBJS)
	$(CC) -o main $(OBJS) $(FLAGS)

./memmap/memmap.o:./memmap/memmap.cpp
	$(CC) -c ./memmap/memmap.cpp $(FLAGS) -o ./memmap/memmap.o

./singleJoin/h1.o:./singleJoin/h1.cpp
	$(CC) -c ./singleJoin/h1.cpp $(FLAGS) -o ./singleJoin/h1.o

./singleJoin/h2.o:./singleJoin/h2.cpp
	$(CC) -c ./singleJoin/h2.cpp $(FLAGS) -o ./singleJoin/h2.o

./singleJoin/join.o:./singleJoin/join.cpp
	$(CC) -c ./singleJoin/join.cpp $(FLAGS) -o ./singleJoin/join.o

./singleJoin/result.o:./singleJoin/result.cpp
	$(CC) -c ./singleJoin/result.cpp $(FLAGS) -o ./singleJoin/result.o

main.o:main.cpp
	$(CC) -c main.cpp $(FLAGS) -o main.o

./singleJoin/structs.o:./singleJoin/structs.cpp
	$(CC) -c ./singleJoin/structs.cpp $(FLAGS) -o ./singleJoin/structs.o

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

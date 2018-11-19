CC = g++
OBJS = ./join/h1.o ./join/h2.o ./join/join.o main.o \
		./join/structs.o ./join/result.o
SERIAL_OBJS = ./join/h1.o ./join/h2.o ./join/join.o \
		./join/structs.o ./join/result.o \
		./testMain/testSerial.o
ODD_EVEN_OBJS = ./join/h1.o ./join/h2.o ./join/join.o \
		./join/structs.o ./join/result.o \
		./testMain/testOddEven.o
FLAGS = -g3 -Wall

all:$(OBJS)
	$(CC) -o joinTest $(OBJS) $(FLAGS)

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

testSerial:$(SERIAL_OBJS)
	$(CC) -o joinTestSerial $(SERIAL_OBJS) $(FLAGS)

./testMain/testSerial.o:./testMain/testSerial.cpp
	$(CC) -c ./testMain/testSerial.cpp $(FLAGS) -o ./testMain/testSerial.o

testOddEven:$(ODD_EVEN_OBJS)
	$(CC) -o joinTestOddEven $(ODD_EVEN_OBJS) $(FLAGS)

./testMain/testOddEven.o:./testMain/testOddEven.cpp
	$(CC) -c ./testMain/testOddEven.cpp $(FLAGS) -o ./testMain/testOddEven.o

clean:
	rm -rf ./*/*.o *.o ./*/*/*.o a.out joinTest ./joinTestSerial \
		./joinTestOddEven ./*/*.gch *.gch ./*/*/*.gch

CC = g++
OBJS = main.o ./join/memmap.o ./join/stringList.o ./parse/parse.o \
		./singleJoin/result.o ./singleJoin/structs.o ./join/inputManager.o \
		./join/intermediate.o
SERIAL_OBJS = ./singleJoin/h1.o ./singleJoin/h2.o ./singleJoin/join.o \
		./singleJoin/structs.o ./singleJoin/result.o \
		./testMain/serialJoin.o
ODD_EVEN_OBJS = ./singleJoin/h1.o ./singleJoin/h2.o ./singleJoin/join.o \
		./singleJoin/structs.o ./singleJoin/result.o \
		./testMain/oddEvenJoin.o
RANDOM_OBJS = ./singleJoin/h1.o ./singleJoin/h2.o ./singleJoin/join.o \
		./singleJoin/structs.o ./singleJoin/result.o \
		./testMain/randomJoin.o
RESULT_OBJS = ./singleJoin/result.o ./testMain/resultTest.o \
			 ./singleJoin/structs.o ./join/intermediate.o
PARSE_OBJS = ./testMain/testParse.o ./parse/parse.o

FLAGS = -g3 -Wall

all:$(OBJS)
	$(CC) -o main $(OBJS) $(FLAGS)

./join/stringList.o:./join/stringList.cpp
	$(CC) -c ./join/stringList.cpp $(FLAGS) -o ./join/stringList.o

./join/memmap.o:./join/memmap.cpp
	$(CC) -c ./join/memmap.cpp $(FLAGS) -o ./join/memmap.o

./join/inputManager.o:./join/inputManager.cpp
	$(CC) -c ./join/inputManager.cpp $(FLAGS) -o ./join/inputManager.o

./join/intermediate.o:./join/intermediate.cpp
	$(CC) -c ./join/intermediate.cpp $(FLAGS) -o ./join/intermediate.o

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

resultTest:$(RESULT_OBJS)
	$(CC) -o resultTest $(RESULT_OBJS) $(FLAGS)

./testMain/resultTest.o:./testMain/resultTest.cpp
	$(CC) -c ./testMain/resultTest.cpp $(FLAGS) -o ./testMain/resultTest.o

./testMain/testParse.o:./testMain/testParse.cpp
	$(CC) -c ./testMain/testParse.cpp $(FLAGS) -o ./testMain/testParse.o

./parse/parse.o:./parse/parse.cpp
	$(CC) -c ./parse/parse.cpp $(FLAGS) -o ./parse/parse.o

testParse:$(PARSE_OBJS)
	$(CC) -o testParse $(PARSE_OBJS) $(FLAGS)

clean:
	rm -rf ./*/*.o *.o ./*/*/*.o a.out main randomJoin serialJoin testParse \
		oddEvenJoin resultTest ./*/*.gch *.gch ./*/*/*.gch

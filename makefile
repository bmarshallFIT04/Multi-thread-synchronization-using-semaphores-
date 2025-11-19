all: main cse4001_sync
main:
	g++ -g main.cpp -o main -lpthread -lm 
cse4001_sync:
	g++ -g sync.cpp -o cse4001_sync -lpthread -lm
clean:
	rm -f main cse4001_sync
	
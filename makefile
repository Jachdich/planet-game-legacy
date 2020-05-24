run: client server
	./server &
	./client

client:
	g++ client.cpp FastNoise.cpp -Wall -g -o $@ -Bstatic -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp

server:
	g++ server.cpp -lpthread -ljsoncpp -g -Wall -o $@

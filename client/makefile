client:
	g++ client.cpp FastNoise.cpp -Wall -g -o $@ -Bstatic -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp

run: client
	./client

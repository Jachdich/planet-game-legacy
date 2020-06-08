SOURCES = client.cpp FastNoise.cpp planet.cpp star.cpp sector.cpp sectorcache.cpp game.cpp network.cpp
HEADERS = $(SOURCES:.cpp=.h)
OBJECTS = $(SOURCES:.cpp=.o)

client: $(OBJECTS)
	g++ $(OBJECTS) -o $@ -lX11 -lGL -lpthread -lpng -lstdc++fs -ljsoncpp

%.o: %.cpp $(HEADERS)
	g++ -c -o $@ $< -Wall -g

debug: client
	gdb client

run: client
	./client

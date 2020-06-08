SOURCES = server.cpp generation.cpp logging.cpp planet.cpp sector.cpp star.cpp sectormap.cpp

server: $(SOURCES)
	g++ $(SOURCES) -lpthread -ljsoncpp -g -Wall -o $@

run: server
	./server

debug: server
	gdb server

CXX=g++
CFLAGS=-std=c++14 -Wall -Wextra
SRC_SERVER=$(shell find ./src/server -name "*.cc")
SRC_CLIENT=$(shell find ./src/client -name "*.cc")
OBJ_SERVER=$(subst .cc,.o,$(SRC_SERVER))
OBJ_CLIENT=$(subst .cc,.o,$(SRC_CLIENT))

all: server client

server: $(OBJ_SERVER) $(OBJ_UTILS) src/server/server.h
	$(CXX) $(CFLAGS) -c -o src/server/main.o src/server/main.cc
	$(CXX) $(CFLAGS) -o server $(OBJ_SERVER) $(OBJ_UTILS)

client: $(OBJ_CLIENT) $(OBJ_UTILS) src/client/client.h
	$(CXX) $(CFLAGS) -o client $(OBJ_CLIENT) $(OBJ_UTILS)

%.o: %.cc
	$(CXX) $(CFLAGS) -c -o $@ $<

clean: clean-client clean-server clean-database
	rm -rf $(shell find . -name "*.o")

clean-client:
	rm -rf $(shell find src/client -name "*.o")
	rm -f ./client

clean-server:
	rm -rf $(shell find src/server -name "*.o")
	rm -f ./server

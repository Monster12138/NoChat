cc=g++
server=ServerNoChat
client=ClientNoChat
libdir=./lib
.PHONY: all
all:clean $(server) $(client)
$(server):ServerNoChat.cc
	$(cc) -o $@ $^ -pthread -L$(libdir) -g -ljsoncpp -static
$(client):ClientNoChat.cc
	$(cc) -o $@ $^ -pthread -lncurses -L$(libdir) -g -ljsoncpp
.PHONY: clean
clean:
	rm -rf ServerNoChat ClientNoChat

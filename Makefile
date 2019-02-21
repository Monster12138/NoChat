cc=g++
server=ServerNoChat
client=ClientNoChat
.PHONY:all
all:$(server) $(client)
$(server):ServerNoChat.cc
	$(cc) -o $@ $^ -pthread
$(client):ClientNoChat.cc
	$(cc) -o $@ $^ -pthread

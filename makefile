compile: client1 server1

client1: client.o
	@gcc -Wall -o clientfile client.o

client.o: client.c shared.h
	@gcc -Wall -c client.c

play: clientfile
	@./clientfile

server1: server.o
	@gcc -Wall -o serverfile server.o

server.o: server.c shared.h
	@gcc -Wall -c server.c

server: serverfile
	@./serverfile

clean:
	@rm -f *.o clientfile serverfile *~ server_pipe client_pipe_*

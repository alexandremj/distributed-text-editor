run:
	gcc server.c -o server.run
	gcc client.c -o client.run
	gcc client-copy.c -o client-copy.run
	./server.run

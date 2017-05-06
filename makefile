all:
	gcc -o server server.c
	gcc -o client client.c
	gcc -o bridge bridge.c
clean:
	rm -f bridge
	rm -f server
	rm -f client

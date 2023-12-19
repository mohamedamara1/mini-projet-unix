# Makefile for client and server compilation

CC = gcc
CFLAGS = -Wall -Wextra -pedantic
GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0)

all: server client

server: tcp_server.c
	$(CC) $(CFLAGS) -o server tcp_server.c -lpthread

client: tcp_client.c
	$(CC) $(CFLAGS) -o client tcp_client.c $(GTK_FLAGS)

clean:
	rm -f server client

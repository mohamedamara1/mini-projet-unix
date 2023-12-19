CC = gcc
CFLAGS = -Wall -Iinclude

SERVER_EXEC = serveurTCP_fork
CLIENT_EXEC = clientTCP_fork

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): ./serveurTCP_fork.c
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_EXEC): ./clientTCP_fork.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC)

.PHONY: all clean

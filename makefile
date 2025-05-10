CC = gcc

CFLAGS = -Wall -Wextra -pedantic -std=c11 -pthread

OUTPUT = server

SOURCE_FILES = server.c request.c response.c handler.c

all: $(OUTPUT)

$(OUTPUT): $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCE_FILES)

clean:
	rm -f $(OUTPUT)

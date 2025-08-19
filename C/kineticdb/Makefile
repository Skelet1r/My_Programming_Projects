CC=gcc
CFLAGS=-Wall -g -c
INCLUDES=-Iincludes
OBJ=commands.o commands_parser.o exec_commands.o main.o utils.o word_item.o
SYS_PATH=/usr/local
BIN_DIR=$(SYS_PATH)/bin
DATA_DIR=/var/lib/kineticdb

all: kineticdb

kineticdb: $(OBJ) 
	$(CC) $(OBJ) -o kineticdb

commands.o: commands.c
	$(CC) $(CFLAGS) $(INCLUDES) commands.c

commands_parser.o: commands_parser.c
	$(CC) $(CFLAGS) $(INCLUDES) commands_parser.c

exec_commands.o: exec_commands.c
	$(CC) $(CFLAGS) $(INCLUDES) exec_commands.c

main.o: main.c
	$(CC) $(CFLAGS) $(INCLUDES) main.c

utils.o: utils.c
	$(CC) $(CFLAGS) $(INCLUDES) utils.c

word_item.o: word_item.c
	$(CC) $(CFLAGS) $(INCLUDES) word_item.c

clean:
	rm -f *.o

install: kineticdb
	install -d $(BIN_DIR)
	install kineticdb $(BIN_DIR)/kineticdb
	touch $(DATA_DIR)

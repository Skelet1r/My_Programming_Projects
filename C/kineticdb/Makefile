CC=gcc
CFLAGS=-Wall -g -c
INCLUDES=-Iincludes
OBJ=commands.o commands_parser.o exec_commands.o main.o utils.o word_item.o
USER=kineticdb
SYS_PATH=/var/lib/kineticdb
BIN_PATH=/usr/local/bin

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
	id -u $(USER) >/dev/null 2>&1 || sudo useradd -r -s /bin/false $(USER)
	sudo mkdir -p $(SYS_PATH)
	sudo chown $(USER):$(USER) $(SYS_PATH)
	sudo cp kineticdb $(BIN_PATH)/
	sudo chmod 755 /usr/local/bin/kineticdb

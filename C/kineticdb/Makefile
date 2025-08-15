all: kineticdb_bin

kineticdb_bin: commands.o commands_parser.o exec_commands.o main.o utils.o word_item.o
	gcc -Wall -g commands.o commands_parser.o exec_commands.o main.o utils.o word_item.o -o kineticdb_bin

commands.o: commands.c
	gcc -Wall -g -Iincludes -c commands.c

commands_parser.o: commands_parser.c
	gcc -Wall -g -Iincludes -c commands_parser.c

exec_commands.o: exec_commands.c
	gcc -Wall -g -Iincludes -c exec_commands.c

main.o: main.c
	gcc -Wall -g -Iincludes -c main.c

utils.o: utils.c
	gcc -Wall -g -Iincludes -c utils.c

word_item.o: word_item.c
	gcc -Wall -g -Iincludes -c word_item.c

clean:
	rm -f *.o kineticdb_bin

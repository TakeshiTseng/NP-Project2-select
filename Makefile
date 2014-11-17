CC=gcc

all:
	$(CC) -g main.c ./pipe_node.c ./cmd_node.c ./node_list.c ./parser.c ./server.c ./token.c ./tokenizer.c ./util.c -o ras.o

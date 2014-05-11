
NAME:=main

SOURCES:= main.c  \
		  node.c
CFLAGS:=-std=c99

$(NAME): $(SOURCES)
	gcc $(SOURCES) -g -o $(NAME) $(CFLAGS)

clean:
	rm -rf $(NAME)

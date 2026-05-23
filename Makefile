CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS	=	-lreadline	-lhistory

TARGET = tuffshell

SRCS = main.c src/builtins.c src/executor.c src/parser.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)	$(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install:	$(TARGET)
	sudo	cp	$(TARGET)	/usr/local/bin/tuff

uninstall:
	sudo	rm	-f	/usr/local/bin/tuff

.PHONY: all	clean	install	uninstall

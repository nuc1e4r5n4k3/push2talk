

all: push2talk


push2talk: push2talk.o
	gcc -o $@ $^

push2talk.o: push2talk.c
	gcc -o $@ -c $^

clean:
	rm -f push2talk push2talk.o

install:
	install -o root -g root -m 0755 push2talk /usr/local/bin/push2talk

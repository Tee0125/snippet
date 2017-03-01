DEBUG = -DDEBUG -g
CFLAGS = -O3 -pipe -Wall
LDFLAGS = -lm

OBJS = test.o gaussian.o image.o 
all: gaussian

.c.o: 
	gcc -c ${DEBUG} ${CFLAGS} $<

gaussian: ${OBJS}
	gcc -o gaussian ${LDFLAGS} ${OBJS}
clean: 
	rm gaussian ${OBJS}

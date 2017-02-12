DEBUG = -DDEBUG -g
CFLAGS = -O3 -pipe -Wall
LDFLAGS = -lm

OBJS = matrix.o projection.o
all: projection

.c.o: 
	gcc -c ${DEBUG} ${CFLAGS} $<

projection: ${OBJS}
	gcc -o projection ${LDFLAGS} ${OBJS}
clean: 
	rm projection ${OBJS}

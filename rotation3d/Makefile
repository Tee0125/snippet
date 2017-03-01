#DEBUG = -DDEBUG -g
CFLAGS = -O3 -pipe -Wall

OBJS = rotation3d.o

all: rotation3d

.c.o: 
	gcc -c ${DEBUG} $<

rotation3d: ${OBJS}
	gcc -o rotation3d -lm ${OBJS}

test: rotation3d 
	./rotation3d

clean: 
	rm -rf ${OBJS} rotation3d 

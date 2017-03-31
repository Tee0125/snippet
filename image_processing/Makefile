#DEBUG = -DDEBUG
CFLAGS = -O3 -pipe -g 
LDFLAGS = -lgd -lpng -lz -ljpeg -lfreetype -lm 

OBJS = filter.o image.o image_processing.o html.o
.c.o:
	gcc -c ${CFLAGS} ${DEBUG} $<

all: ip 

ip: ${OBJS}
	gcc -o ip ${LDFLAGS} ${OBJS}

clean: 
	rm ${OBJS}

test: ip
	./ip

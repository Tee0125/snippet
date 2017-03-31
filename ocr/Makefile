#DEBUG = -DDEBUG
CFLAGS = -O3 -pipe -g 
LDFLAGS = -lgd -lpng -lz -ljpeg -lfreetype -lm 

OBJS = ocr.o filter.o image.o image_processing.o
.c.o:
	gcc -c ${CFLAGS} ${DEBUG} $<

all: image_processing 

image_processing: ${OBJS}
	gcc -o image_processing ${LDFLAGS} ${OBJS}

clean: 
	rm ${OBJS}

ft: ft2.o
	gcc -o ft ${LDFLAGS} ft2.o

test: image_processing
	./image_processing

# DEBUG = -DDEBUG
# CFLAGS = -march=pentium3 -O3 -pipe
# 
CFLAGS  := -O2 -pipe
LDFLAGS := 

.c.o:
	gcc -c ${CFLAGS} ${DEBUG} $<

lifegame:  lifegame.o
	gcc ${LDFLAGS} -o $@ $<
	
clean:
	rm -rf lifegame.o
	rm -rf lifegame.out
	rm -rf lifegame

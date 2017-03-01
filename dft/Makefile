# DEBUG = -DDEBUG
# 
CFLAGS  := -O2 -pipe
LDFLAGS := 

.cpp.o:
	g++ -c ${CFLAGS} ${DEBUG} $<

dft:  dft.o
	g++ ${LDFLAGS} -o $@ $<
	
clean:
	rm -rf dft.o
	rm -rf dft

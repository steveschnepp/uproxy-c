all: uproxy-c

uproxy-c: uproxy-c.o lib/mkpath.o

clean:
	rm -f *.o lib/*.o uproxy-c

.PHONY: clean all

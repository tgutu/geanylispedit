all: geanylispedit.so
	
geanylispedit.so: geanylispedit.o
	gcc geanylispedit.o -o geanylispedit.so -shared `pkg-config --libs geany` 

geanylispedit.o: geanylispedit.c geanylispedit.h
	gcc -c geanylispedit.c -fPIC `pkg-config --cflags geany`

clean:
	rm -rf *.o *.so
	
install: geanylispedit.so
	sudo cp geanylispedit.so /usr/lib/geany/geanylispedit.so
	
uninstall:
	sudo rm /usr/lib/geany/geanylispedit.so
	

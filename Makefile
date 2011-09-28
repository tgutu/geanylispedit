all: geanylisp.so
	
geanylisp.so: geanylisp.o
	gcc geanylisp.o -o geanylisp.so -shared `pkg-config --libs geany` 

geanylisp.o: geanylisp.c
	gcc -c geanylisp.c -fPIC `pkg-config --cflags geany`

clean:
	rm -rf *.o *.so
	
install: geanylisp.so
	sudo cp geanylisp.so /usr/lib/geany/geanylisp.so
	
uninstall:
	sudo rm /usr/lib/geany/geanylisp.so
	

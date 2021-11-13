main.exe: main.o compiler.o
	gcc src/main.o src/compiler.o -o main.exe
main.o: src/main.c
	gcc -c src/main.c -o src/main.o
compiler.o: src/compiler.c
	gcc -c src/compiler.c -o src/compiler.o

clean:
	rm -f src/*.o *~ 
main.exe: main.o parser.o tokenizer.o
	gcc src/main.o src/parser.o src/tokenizer.o -o main.exe
#main.exe: main.o compiler.o
#	gcc src/main.o src/compiler.o -o main.exe
main.o: src/main.c
	gcc -c src/main.c -o src/main.o
#compiler.o: src/compiler.c
#	gcc -c src/compiler.c -o src/compiler.o

parser.o: src/parser.c
	gcc -c src/parser.c -o src/parser.o

tokenizer.o: src/tokenizer.c
	gcc -c src/tokenizer.c -o src/tokenizer.o

clean:
	rm -f src/*.o *~ 
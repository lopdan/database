database: main.o parser.o tokenizer.o
	gcc src/main.o src/parser.o src/tokenizer.o -o database

main.o: src/main.c
	gcc -c src/main.c -o src/main.o

parser.o: src/parser.c
	gcc -c src/parser.c -o src/parser.o

tokenizer.o: src/tokenizer.c
	gcc -c src/tokenizer.c -o src/tokenizer.o

clean:
	rm -f src/*.o *~ 
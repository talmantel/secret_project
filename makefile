all: actions.o data.o entries.o errors.o extern.o first_pass.o words.o labels.o linked_list.o main.o output.o parser.o second_pass.o symbols.o
	gcc -g -ansi -pedantic -Wall actions.o data.o entries.o errors.o extern.o first_pass.o words.o labels.o linked_list.o main.o output.o parser.o second_pass.o symbols.o -o assembler


main.o: main.c definitions.h linked_list.h first_pass.h second_pass.h output.h data.h entries.h words.h symbols.h errors.h extern.h
	gcc -c -ansi -pedantic -Wall main.c


second_pass.o: second_pass.c second_pass.h definitions.h errors.h linked_list.h symbols.h words.h entries.h extern.h
	gcc -c -ansi -pedantic -Wall second_pass.c

first_pass.o: first_pass.c first_pass.h parser.h symbols.h errors.h definitions.h linked_list.h
	gcc -c -ansi -pedantic -Wall first_pass.c

output.o: output.c output.h definitions.h errors.h words.h entries.h extern.h data.h linked_list.h
	gcc -c -ansi -pedantic -Wall output.c
 
parser.o: parser.c parser.h actions.h symbols.h words.h data.h entries.h errors.h labels.h linked_list.h definitions.h
	gcc -c -ansi -pedantic -Wall parser.c

actions.o: actions.c actions.h errors.h labels.h words.h definitions.h
	gcc -c -ansi -pedantic -Wall actions.c

labels.o: labels.c labels.h errors.h definitions.h
	gcc -c -ansi -pedantic -Wall labels.c
linked_list.o: linked_list.c linked_list.h errors.h
	gcc -c -ansi -pedantic -Wall linked_list.c
data.o: data.c data.h
	gcc -c -ansi -pedantic -Wall data.c
entries.o: entries.c entries.h
	gcc -c -ansi -pedantic -Wall entries.c
errors.o: errors.c errors.h
	gcc -c -ansi -pedantic -Wall errors.c
extern.o: extern.c extern.h
	gcc -c -ansi -pedantic -Wall extern.c
words.o: words.c words.h
	gcc -c -ansi -pedantic -Wall words.c
symbols.o: symbols.c symbols.h definitions.h
	gcc -c -ansi -pedantic -Wall symbols.c

clear:
	rm actions.o data.o entries.o errors.o extern.o first_pass.o words.o labels.o linked_list.o main.o output.o parser.o second_pass.o symbols.o assembler

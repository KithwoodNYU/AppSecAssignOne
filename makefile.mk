
CFLAGS=-Wall
LIBS=-lcheck -lm -lrt -lpthread -lsubunit

all: spell_check test

spell_check : main.o spell.o dictionary.o 
	gcc -o spell_check sabrina_spell_main.o spell.o dictionary.o

main.o : sabrina_spell_main.c 
	gcc $(CFLAGS) -c sabrina_spell_main.c

spell.o : spell.c
	gcc $(CFLAGS) -c spell.c

dictionary.o : dictionary.c
	gcc $(CFLAGS) -c dictionary.c

testmain.o: test_main.c
	gcc $(CFLAGS) -c test_main.c

test : testmain.o spell.o dictionary.o
	gcc -o test_spell_check test_main.o spell.o dictionary.o $(LIBS)
	./test_spell_check

fuzz_main.o: sabrina_spell_main.c
	afl-gcc $(CFLAGS) -c sabrina_spell_main.c

fuzz_spell.o : spell.c
	afl-gcc $(CFLAGS) -c spell.c

fuzz_dictionary.o : dictionary.c
	afl-gcc $(CFLAGS) -c dictionary.c

fuzz : fuzz_main.o fuzz_spell.o fuzz_dictionary.o
	afl-gcc -o afl_test sabrina_spell_main.o spell.o dictionary.o $(LIBS)
	
clean : 
	rm spell_check sabrina_spell_main.o spell.o dictionary.o

fuzz_clean: 
	rm afl_test sabrina_spell_main.o spell.o dictionary.o
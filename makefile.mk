spell_check : main.o spell.o dictionary.o 
	cc -g -o spell_check sabrina_spell_main.o spell.o dictionary.o

main.o : sabrina_spell_main.c 
	cc -g -c sabrina_spell_main.c

spell.o : spell.c
	cc -g -c spell.c

dictionary.o : dictionary.c
	cc -g -c dictionary.c

clean : 
	rm spell_check sabrina_spell_main.o spell.o dictionary.o

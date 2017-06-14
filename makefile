all: y86emul y86dis

y86emul: y86emul.c
	gcc -Wall -lm -o y86emul y86emul.c

y86dis: y86dis.c
	gcc -Wall -lm -o y86dis y86dis.c

clean:
	rm -f *.o y86emul y86dis
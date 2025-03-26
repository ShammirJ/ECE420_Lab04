# FRIENDLY REMIDER: NEVER DO rm *.txt

EXES = datatrim serial main randomTester
TXT = data_input_meta data_input_link data_output

reset: cleanall all

all: $(EXES)

randomTester: randomTester.c
	gcc randomTester.c -o randomTester

main: main.o Lab4_IO.o
	mpicc -Wall -g -lm -o main main.o Lab4_IO.o -lm

main.o: main.c
	mpicc -Wall -g -lm -c main.c -o main.o -lm

serial: serial.o Lab4_IO.o
	gcc -Wall -g -lm -o serial serial.o Lab4_IO.o -lm

serial.o: main_serial.c
	gcc -Wall -g -lm -c main_serial.c -o serial.o -lm

Lab4_IO.o: Lab4_IO.c Lab4_IO.h
	gcc -Wall -g -lm -c Lab4_IO.c -o Lab4_IO.o -lm

datatrim: datatrim.c
	gcc datatrim.c -o datatrim

cleanall: clean cleantxt

clean:
	rm -f *.o $(EXES)

cleantxt:
	rm -f *.log $(TXT)

zip:
	zip user30_lab04 *

cleanzip:
	rm *.zip
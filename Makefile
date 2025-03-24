# FRIENDLY REMIDER: NEVER DO rm *.txt

all: datatrim serial

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
	rm -f *.o datatrim serial

cleantxt:
	rm -f data_input_meta data_input_link data_output

all: encode decode bs_unittest gr_unittest

clean:
	rm -f *.o encode decode out.txt canon.txt out.bit dec.log enc.log

NMEA=$(wildcard *.NMEA)

encode.o: encode.c deltacode.h bitstream.h grcode.h
grcode.o: grcode.c bitstream.h
deltacode.o: deltacode.c grcode.h

diff: canon.txt out.txt
	./diff.pl canon.txt out.txt

out.txt: out.bit decode
	./decode out.bit out.txt >dec.log

test: canon.txt out.txt #test1 test2
	tail -5 canon.txt
	tail -5 out.txt

test1: bs_unittest
	./ut_bs

test2: gr_unittest
	echo  34567789 1 2 4 6 7 20 34567789 > /tmp/t.txt
	./gr_unittest /tmp/t.txt 3

encode: encode.o bitstream.o grcode.o deltacode.o
	gcc -g -o encode $^ -lm 

decode: decode.o bitstream.o grcode.o deltacode.o
	gcc -g -o decode $^ -lm 

canon.txt: check.pl $(NMEA)
	./check.pl < $(NMEA) > canon.txt

out.bit: encode canon.txt
	./encode canon.txt out.bit 3 >enc.log


bs_unittest: bs_unittest.o bitstream.o
	gcc -g -o bs_unittest $^ -lm

gr_unittest: gr_unittest.o bitstream.o grcode.o
	gcc -g -o gr_unittest $^ -lm

%.o: %.c
	gcc -g -Wall -Werror -c $<

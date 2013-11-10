all: encode.exe decode.exe bs_unittest.exe gr_unittest.exe

NMEA=$(wildcard *.NMEA)

encode.o: encode.c deltacode.h bitstream.h grcode.h
grcode.o: grcode.c bitstream.h
deltacode.o: deltacode.c grcode.h

diff: out.txt
	./diff.pl canon.txt out.txt

out.txt: out.bit decode.exe
	./decode out.bit out.txt >dec.log

test: out.txt #test1 test2
	tail -5 canon.txt
	tail -5 out.txt

test1: bs_unittest.exe
	./ut_bs

test2: gr_unittest.exe
	echo  34567789 1 2 4 6 7 20 34567789 > /tmp/t.txt
	./gr_unittest /tmp/t.txt 3

encode.exe: encode.o bitstream.o grcode.o deltacode.o
	gcc -g -o encode $^

decode.exe: decode.o bitstream.o grcode.o deltacode.o
	gcc -g -o decode $^

canon.txt: check.pl $(NMEA)
	./check.pl < $(NMEA) > canon.txt

out.bit: encode.exe canon.txt
	./encode.exe canon.txt out.bit 3 >enc.log


bs_unittest.exe: bs_unittest.o bitstream.o
	gcc -g -o bs_unittest $^

gr_unittest.exe: gr_unittest.o bitstream.o grcode.o
	gcc -g -o gr_unittest $^

%.o: %.c
	gcc -g -c $<
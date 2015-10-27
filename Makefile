CC=cmoc
COPTS=--verbose --org=C000 -DCXCOMM_TEST=1 -DDEBUG=1



test: test.c 
	$(CC) $(COPTS) test.c 

#xmodem:	xmodem.c
#	$(CC) $(COPTS) xmodem.c

copy: test
	-- imgtool del coco_jvc_rsdos disk.dsk T.BIN
	imgtool put coco_jvc_rsdos disk.dsk test.bin T.BIN 

debug: copy 
	mess coco3 -flop1 ./disk.dsk -debug -resolution 320x300

run:	copy
	mess coco3 -flop1 ./disk.dsk -window -resolution 320x300


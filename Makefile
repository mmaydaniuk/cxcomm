CC=cmoc
COPTS=--no-peephole


test: test.c 
	$(CC) $(COPTS) test.c 

#xmodem:	xmodem.c
#	$(CC) $(COPTS) xmodem.c

copy: test
	imgtool del coco_jvc_rsdos disk.dsk TEST.BIN
	imgtool put coco_jvc_rsdos disk.dsk test.bin TEST.BIN 

debug: copy 
	mess coco3 -flop1 ./disk.dsk -debug

run:	copy
	mess coco3 -flop1 ./disk.dsk -window


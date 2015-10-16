CC=cmoc
COPTS=--org=C000

xmodem:	xmodem.c
	$(CC) $(COPTS) xmodem.c

copy: xmodem xmodem.bin
	- imgtool del coco_jvc_rsdos cocotest1.dsk XMODEM.BIN
	imgtool put coco_jvc_rsdos cocotest1.dsk xmodem.bin XMODEM.BIN

test: copy 
	mess coco3 -flop1 ./cocotest1.dsk -debug



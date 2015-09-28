# cxcomm
Communications Tools for TRS-80 Color Computer 3

This package provides a communications driver (in the same vein as COTERM.BIN) and XMODEM/YMODEM transmit and receive commands. The application has been developed with Pierre Sarazin's excellent cmoc c-like compiler, and is implemented in C and 6809 assembly language.

Compiling
=========

Make sure you have the cmoc compiler installed.

From the main project directory, run "make".

If you have a Coco 3 emulator installed with M.E.S.S., the "test" target in the Makefile will launch the emulator with file XMODEM.BIN in the disk image.

Commands
========

The application adds the following commands to RSDOS:

RX"FILENAME.EXT"  - Receive an file using XMODEM.
RY                - Receive one or more files using YMODEM-1k
SX"FILENAME.EXT"  - Send a file using XMODEM.
SY"FILESPEC"      - Send a file using YMODEM.   Wildcards (?, *) are allowed to send multiple files.

HOST ON           - Enable RS-232 communications.  PRINT, INPUT, etc will work from either the comm port, or console.
HOST OFF          - Disable RS-232 communications.



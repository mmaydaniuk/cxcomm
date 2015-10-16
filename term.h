#ifndef CXCOMM_TERM_H
#define CXCOMM_TERM_H

// turn on and off console over serial
void term_host();
void term_redraw();
void term_install();
void term_putc(char c);
int term_getc();

#endif

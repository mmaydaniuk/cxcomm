#ifndef PROTOCOL_H
uint16_t
uint16_t crc_xmodem_update (uint16_t crc, uint8_t data)

void xmodem_send(char* filename);
void ymodem_send(char* filename);
void xmodem_receive(char* filename);
void ymodem_receive();

#endif


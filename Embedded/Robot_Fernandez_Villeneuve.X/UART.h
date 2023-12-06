/* 
 * File:   UART.h
 * Author: GEII Robot
 *
 * Created on 6 décembre 2023, 16:12
 */

#ifndef UART_H
#define UART_H

void InitUART(void);
void SendMessageDirect(unsigned char* message, int length);

#endif /* UART_H */


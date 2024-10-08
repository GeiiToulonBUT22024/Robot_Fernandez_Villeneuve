/* 
 * File:   CB_TP1.h
 * Author: GEII Robot
 *
 * Created on 6 décembre 2023, 17:46
 */

#ifndef CB_TP1_H
#define	CB_TP1_H

void SendMessage(unsigned char* message, int length);
void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get(void);
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void);
void SendOne();
int CB_TX1_GetDataSize(void);
int CB_TX1_GetRemainingSize(void);
unsigned char CB_TX1_IsTranmitting(void);

#endif	/* CB_TP1_H */


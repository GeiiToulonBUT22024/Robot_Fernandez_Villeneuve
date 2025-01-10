#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_RX2.h"
#define CBRX2_BUFFER_SIZE 128
int cbRx2Head;
int cbRx2Tail;
unsigned char cbRx2Buffer[CBRX2_BUFFER_SIZE];

void CB_RX2_Add(unsigned char value) {
    if (CB_RX2_GetRemainingSize() > 0) {
        cbRx2Buffer[cbRx2Head] = value;
        cbRx2Head++;
        if (cbRx2Head > CBRX2_BUFFER_SIZE - 1) {
            cbRx2Head = 0;
        }
    }
}

unsigned char CB_RX2_Get(void) {
    unsigned char value = cbRx2Buffer[cbRx2Tail];
    cbRx2Tail ++;
    if (cbRx2Tail > CBRX2_BUFFER_SIZE - 1) {
            cbRx2Tail = 0;
        }
    return value;
}

unsigned char CB_RX2_IsDataAvailable(void) {
    if (cbRx2Head != cbRx2Tail)
        return 1;
    else
        return 0;
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
    IFS1bits.U2RXIF = 0; // clear RX interrupt flag
    /* check for receive errors */
    if (U2STAbits.FERR == 1) {
        U2STAbits.FERR = 0;
    }
    /* must clear the overrun error to keep uart receiving */
    if (U2STAbits.OERR == 1) {
        U2STAbits.OERR = 0;
    }
    /* get the data */
    while (U2STAbits.URXDA == 1) {
        CB_RX2_Add(U2RXREG);
    }
}

int CB_RX2_GetDataSize(void) {
    //return size of data stored in circular buffer
    int dataSize;
    if (cbRx2Tail <= cbRx2Head) {
        dataSize = cbRx2Head - cbRx2Tail;
    }
    else {
        dataSize = 128 - (cbRx2Tail - cbRx2Head);
    }
    return dataSize;
}

int CB_RX2_GetRemainingSize(void) {
    //return size of remaining size in circular buffer
    int remainingSize = CBRX2_BUFFER_SIZE - CB_RX2_GetDataSize() - 1; 
    return remainingSize;
}

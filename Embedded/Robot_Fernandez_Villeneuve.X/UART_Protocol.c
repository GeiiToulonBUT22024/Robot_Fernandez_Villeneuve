#include <xc.h>
#include "UART_Protocol.h"
#include "main.h"
#include "IO.h"
#include "CB_TX1.h"
#include "Robot.h"
#include "asservissement.h"
#include "Utilities.h"

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction prenant entree la trame et sa longueur pour calculer le checksum
    int checksum = 0;

    checksum ^= 0xFE;

    checksum ^= (msgFunction >> 8);
    checksum ^= (msgFunction);
    checksum ^= msgPayloadLength >> 8;
    checksum ^= (msgPayloadLength);

    for (int i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }

    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction d?encodage et d?envoi d?un message
    unsigned char tabTrame[msgPayloadLength + 6];
    tabTrame[0] = 0xFE;
    tabTrame[1] = msgFunction >> 8;
    tabTrame[2] = msgFunction;
    tabTrame[3] = msgPayloadLength >> 8;
    tabTrame[4] = msgPayloadLength;
    int i = 0;
    for (i = 0; i < msgPayloadLength; i++) {
        tabTrame[5 + i] = msgPayload[i];
    }
    tabTrame[5 + i] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

    SendMessage(tabTrame, sizeof (tabTrame));
}



int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
int rcvState = 0;
unsigned char calculatedChecksum;

void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    switch (rcvState) {
        case Waiting:
            if (c == 0xFE) {
                rcvState = FunctionMSB;
            }
            break;

        case FunctionMSB:
            msgDecodedFunction = c << 8;
            rcvState = FunctionLSB;
            break;

        case FunctionLSB:
            msgDecodedFunction += c;
            rcvState = PayloadLengthMSB;
            break;

        case PayloadLengthMSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = PayloadLengthLSB;
            break;

        case PayloadLengthLSB:
            msgDecodedPayloadLength += c;
            rcvState = Payload;
            break;

        case Payload:

            if (msgDecodedPayloadLength > 1024) {
                rcvState = Waiting;
            } else if (msgDecodedPayloadIndex < msgDecodedPayloadLength) {
                msgDecodedPayload[msgDecodedPayloadIndex] = c;
                msgDecodedPayloadIndex++;

                if (msgDecodedPayloadIndex == msgDecodedPayloadLength) {
                    msgDecodedPayloadIndex = 0;
                    rcvState = CheckSum;
                }
            }
            break;

        case CheckSum:
            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);

            if (calculatedChecksum == c) {
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            } else {
                UartProcessDecodedMessage(0x13, msgDecodedPayloadLength, msgDecodedPayload);
            }
            rcvState = Waiting;
            break;

        default:
            rcvState = Waiting;
            break;
    }
}

void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload) {
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu

    // Case Error 404
    //    if (function == 0x13) {
    //        char array[] = "Error 404";
    //        for (int b = 0; b<sizeof (array); b++) {
    //            ...;
    //        }
    //    }

    // Case 0x20
    if (function == 0x20) {
        int tabLED[payloadLength];

        for (int i = 0; i < payloadLength; i++) {
            tabLED[i] = payload[i];
        }
        if (tabLED[0] == 1) {
            if (tabLED[1] == 0) {
                LED_ORANGE = 0;
            } else if (tabLED[1] == 1) {
                LED_ORANGE = 1;
            }
        } else if (tabLED[0] == 2) {
            if (tabLED[1] == 0) {
                LED_BLEUE = 0;
            } else if (tabLED[1] == 1) {
                LED_BLEUE = 1;
            }
        } else if (tabLED[0] == 3) {
            if (tabLED[1] == 0) {
                LED_BLANCHE = 0;
            } else if (tabLED[1] == 1) {
                LED_BLANCHE = 1;
            }
        }
    }
    
    // Case 0x70
    if(function == 0x70){
        
       float Kp = getFloat(payload, 0);
       float Ki = getFloat(payload, 4);
       float Kd = getFloat(payload, 8); 
       float LimP = getFloat(payload, 12);
       float LimI = getFloat(payload, 16);
       float LimD = getFloat(payload, 20);

       SetupPidAsservissement(&robotState.PidX, Kp, Ki, Kd, LimP, LimI, LimD);
    }
}
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/
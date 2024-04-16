#include <xc.h>
#include "UART_Protocol.h"
#include "main.h"
#include "IO.h"
#include "CB_TX1.h"
#include "Robot.h"
#include "asservissement.h"
#include "Utilities.h"
#include "Trajectoire.h"

extern volatile GhostPosition ghostPosition;

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
    int tabLED[payloadLength];
    int tabPos[payloadLength];
    float KpX, KiX, KdX, LimPX, LimIX, LimDX;
    float KpT, KiT, KdT, LimPT, LimIT, LimDT;

    switch (function) {
        case 0x20:

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
            break;

        case 0x50:
            if(payloadLength == 8)
            {
                ghostPosition.waypointX = getFloat(payload, 0);
                ghostPosition.waypointY = getFloat(payload, 4);
            }            
            break;

//        case 0x51:
//            ghostPosition.waypointX = getFloat(payload, 0);
//            break;
//
//        case 0x52:
//            ghostPosition.waypointY = getFloat(payload, 0);
//            break;

        case 0x70:
            KpX = getFloat(payload, 0);
            KiX = getFloat(payload, 4);
            KdX = getFloat(payload, 8);
            LimPX = getFloat(payload, 12);
            LimIX = getFloat(payload, 16);
            LimDX = getFloat(payload, 20);
            SetupPidAsservissement(&robotState.PidX, KpX, KiX, KdX, LimPX, LimIX, LimDX);
            break;

        case 0x71:
            KpT = getFloat(payload, 0);
            KiT = getFloat(payload, 4);
            KdT = getFloat(payload, 8);
            LimPT = getFloat(payload, 12);
            LimIT = getFloat(payload, 16);
            LimDT = getFloat(payload, 20);
            SetupPidAsservissement(&robotState.PidTheta, KpT, KiT, KdT, LimPT, LimIT, LimDT);
            break;

        case 0x80:
            robotState.mode = payload[0];
            break;

        case 0x90:
            robotState.vitesseConsigneLineaire = getFloat(payload, 0);
            break;

        case 0x91:
            robotState.vitesseConsigneAngulaire = getFloat(payload, 0);
            break;
    }
}
//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/

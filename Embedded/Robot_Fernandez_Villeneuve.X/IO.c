/*
 * File:   IO.c
 */

#include <xc.h>
#include "IO.h"
//#include "main.h"

void InitIO() {
    // IMPORTANT : d�sactiver les entr�es analogiques, sinon on perd les entr�es num�riques
    ANSELA = 0; // 0 desactive
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELF = 0;
    ANSELG = 0;

    // Configuration des sorties

    //******* LED ***************************
    _TRISC10 = 0; // LED Orange
    _TRISG6 = 0; //LED Blanche
    _TRISG7 = 0; // LED Bleue

    //****** Moteurs ************************

    // Configuration des entr�es
    _TRISB14 = 0;
    _TRISB15 = 0;
    _TRISC6 = 0;
    _TRISC7 = 0;

    //*************************************************************
    // Unlock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));

    //Assignation des remappable pins

    _U1RXR = 24; //Remappe la RPI24 sur l?�entre Rx1
    _RP36R = 0b00001; //Remappe la sortie Tx1 vers RP36
    
  // Configuration des pins remappables  
    //******************** QEI *****************
    _QEA2R = 97; //assign QEI A to pin RP97
    _QEB2R = 96; //assign QEI B to pin RP96
    _QEA1R = 70; //assign QEI A to pin RP70
    _QEB1R = 69; //assign QEI B to pin RP69
    //*************************************************************
    // Lock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON | (1 << 6));
}


/*
 * File:   IO.c
 */

#include <xc.h>
#include "IO.h"

void InitIO() {
    //****************************************************************************************************/
    // Declaration des pin Analogiques
    //****************************************************************************************************/
    ANSELA = 0; //Desactivation de toutes entree analogique
    ANSELB = 0; //Desactivation de toutes entree analogique
    ANSELD = 0; //Desactivation de toutes entree analogique
    ANSELC = 0; //Desactivation de toutes entree analogique
    ANSELE = 0; //Desactivation de toutes entree analogique
    ANSELG = 0; //Desactivation de toutes entree analogique

    // Configuration des sorties

    //******* LED ***************************
    _TRISJ6 = 0; // LED Orange1
    _TRISJ5 = 0; //LED Blanche1
    _TRISJ4 = 0; // LED Bleue1
    _TRISJ11 = 0; // LED Rouge1
    _TRISH10 = 0; // LED Verte1

    _TRISK15 = 0; // LED Orange2
    _TRISA0 = 0; //LED Blanche2
    _TRISA9 = 0; // LED Bleue2
    _TRISA10 = 0; // LED Rouge2
    _TRISH3 = 0; // LED Verte2



    //****** Moteurs ************************

    // Configuration des entrées
    _TRISE0 = 0;
    _TRISE1 = 0;
    _TRISE2 = 0;
    _TRISE3 = 0;

    /****************************************************************************************************/
    // Gestion des pin remappables
    /****************************************************************************************************/
    UnlockIO(); // On unlock les registres d'entrées/sorties, ainsi que les registres des PPS

    //Assignation des remappable pins

    _U1RXR = 78; //Remappe la RPI78 sur l?éentre Rx1
    _RP79R = 0b00001; //Remappe la sortie Tx1 vers RP79

    // Configuration des pins remappables  
    //******************** QEI *****************
    _QEA2R = 97; //assign QEI A to pin RP97
    _QEB2R = 96; //assign QEI B to pin RP96
    _QEA1R = 70; //assign QEI A to pin RP70
    _QEB1R = 69; //assign QEI B to pin RP69

    LockIO(); // On lock les registres d'entrées/sorties, ainsi que les registres des PPS
}

void LockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bset OSCCON, #6":: : "w1", "w2", "w3");
}

void UnlockIO() {
    asm volatile ("mov #OSCCON,w1 \n"
            "mov #0x46, w2 \n"
            "mov #0x57, w3 \n"
            "mov.b w2,[w1] \n"
            "mov.b w3,[w1] \n"
            "bclr OSCCON, #6":: : "w1", "w2", "w3");
}



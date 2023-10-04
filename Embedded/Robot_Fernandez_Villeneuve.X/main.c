/* 
 * File:   main.c
 * Author: GEII Robot
 *
 * Created on 13 septembre 2023, 15:20
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l?oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();

    InitTimer23();
    InitTimer1();
    InitADC1();
    InitPWM();
    //    PWMSetSpeed(50,MOTEUR_GAUCHE);
    //    PWMSetSpeed(50,MOTEUR_DROIT);

//    PWMSetSpeedConsigne(50, MOTEUR_GAUCHE);
//    PWMSetSpeedConsigne(50, MOTEUR_DROIT);


    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;

    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        //LED_BLANCHE =! LED_BLANCHE;
        
    } // fin main

}

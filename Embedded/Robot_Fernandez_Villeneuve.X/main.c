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
#include "Robot.h"
#include "main.h"

unsigned int*result;
int var1;
int var2;
int var3;
unsigned char stateRobot;
unsigned char nextStateRobot = 0;
float vitessed = 25;
float vitesseg = 25;

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l'oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entrées sorties
    /****************************************************************************************************/
    InitIO();

    InitTimer23();
    InitTimer1();
    InitTimer4();
    InitADC1();
    InitPWM();
    //    PWMSetSpeed(50,MOTEUR_GAUCHE);
    //    PWMSetSpeed(50,MOTEUR_DROIT);

    //    PWMSetSpeedConsigne(50, MOTEUR_GAUCHE);
    //    PWMSetSpeedConsigne(50, MOTEUR_DROIT);


    //    LED_BLANCHE = 1;
    //    LED_BLEUE = 1;
    //    LED_ORANGE = 1;


    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        //LED_BLANCHE =! LED_BLANCHE;
        ADC1StartConversionSequence();
        //        if( ADCIsConversionFinished()==1)
        //        {
        //            result = ADCGetResult();
        //            var1 = result[0];
        //            var2 = result[1];
        //            var3 = result[2];
        //            ADCClearConversionFinishedFlag();
        //        }

        OperatingSystemLoop();


        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [1])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [0])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremeDroite = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;

            if (robotState.distanceTelemetreExtremeDroite > 30) {
                LED_ORANGE = 1;
            } else {
                LED_ORANGE = 0;
            }
            if (robotState.distanceTelemetreCentre > 30) {
                LED_BLEUE = 1;
            } else {
                LED_BLEUE = 0;
            }
            if (robotState.distanceTelemetreExtremeGauche > 30) {
                LED_BLANCHE = 1;
            } else {
                LED_BLANCHE = 0;
            }
        }

    } // fin main

}

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000) {
                stateRobot = STATE_AVANCE;
            }
            break;
        case STATE_AVANCE:
            PWMSetSpeedConsigne(vitessed, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesseg, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(vitessed, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesseg, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(vitessed, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesseg, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(vitessed, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesseg, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(vitessed, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesseg, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        case STATE_RECULE:
            PWMSetSpeedConsigne(vitessed, MOTEUR_DROIT);
            PWMSetSpeedConsigne(vitesseg, MOTEUR_GAUCHE);
            stateRobot = STATE_RECULE_EN_COURS;
            break;
        case STATE_RECULE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

void SetNextRobotStateInAutomaticMode() {
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //Détermination de la position des obstacles en fonction des télémètres
    if (robotState.distanceTelemetreDroit < 40 &&
            robotState.distanceTelemetreCentre >= 40 &&
            robotState.distanceTelemetreGauche >= 40 &&
            robotState.distanceTelemetreExtremeDroite >= 40 &&
            robotState.distanceTelemetreExtremeGauche >= 40) { //Obstacle à droite
        positionObstacle = OBSTACLE_A_DROITE;
    } else if (robotState.distanceTelemetreDroit < 40 &&
            robotState.distanceTelemetreCentre >= 40 &&
            robotState.distanceTelemetreGauche >= 40 &&
            robotState.distanceTelemetreExtremeDroite < 40 &&
            robotState.distanceTelemetreExtremeGauche >= 40) { //Obstacle à droite et extreme droite
        positionObstacle = OBSTACLE_A_DROITE;
    } else if (robotState.distanceTelemetreDroit >= 40 &&
            robotState.distanceTelemetreCentre >= 40 &&
            robotState.distanceTelemetreGauche < 40 &&
            robotState.distanceTelemetreExtremeDroite >= 40 &&
            robotState.distanceTelemetreExtremeGauche >= 40) { //Obstacle à gauche
        //positionObstacle = OBSTACLE_A_GAUCHE;
        positionObstacle = OBSTACLE_A_DROITE;
    } else if (robotState.distanceTelemetreDroit >= 40 &&
            robotState.distanceTelemetreCentre >= 40 &&
            robotState.distanceTelemetreGauche < 40 &&
            robotState.distanceTelemetreExtremeDroite >= 40 &&
            robotState.distanceTelemetreExtremeGauche < 40) { //Obstacle à gauche et extreme gauche
        //positionObstacle = OBSTACLE_A_GAUCHE;
        positionObstacle = OBSTACLE_A_DROITE;

    } else if (robotState.distanceTelemetreDroit >= 40 &&
            robotState.distanceTelemetreCentre >= 40 &&
            robotState.distanceTelemetreGauche >= 40 &&
            robotState.distanceTelemetreExtremeDroite < 40 &&
            robotState.distanceTelemetreExtremeGauche >= 40) {//obstacle extreme droite
        positionObstacle = OBSTACLE_EXTREMEGAUCHE;
    } else if (robotState.distanceTelemetreDroit >= 40 &&
            robotState.distanceTelemetreCentre >= 40 &&
            robotState.distanceTelemetreGauche >= 40 &&
            robotState.distanceTelemetreExtremeDroite >= 40 &&
            robotState.distanceTelemetreExtremeGauche < 40) {//obstacle extreme gauche
        positionObstacle = OBSTACLE_EXTREMEGAUCHE;
    }
    else if (robotState.distanceTelemetreCentre < 40 && robotState.distanceTelemetreCentre >= 25) { //Obstacle en face
        if (robotState.distanceTelemetreDroit < robotState.distanceTelemetreGauche) {
            positionObstacle = OBSTACLE_A_DROITE;
        } else if (robotState.distanceTelemetreDroit > robotState.distanceTelemetreGauche) {
            //positionObstacle = OBSTACLE_A_GAUCHE;
            positionObstacle = OBSTACLE_A_DROITE;
        }
    } else if (robotState.distanceTelemetreCentre < 25) {
        if (robotState.distanceTelemetreDroit < robotState.distanceTelemetreGauche) {
            //if (timestamp3 > 1000){
            //  timestamp3 =0;
            //while (timestamp3 < 100) {
            positionObstacle = OBSTACLE_EN_FACE_D;
            // }
            // }
        } else if (robotState.distanceTelemetreDroit > robotState.distanceTelemetreGauche) {
            //if (timestamp3 > 1000){
            //  timestamp3 =0;
            //  while (timestamp3 < 100) {
            positionObstacle = OBSTACLE_EN_FACE_G;
            //}
            //}
        }
    } else if (robotState.distanceTelemetreDroit >= 40 &&
            robotState.distanceTelemetreCentre >= 40 &&
            robotState.distanceTelemetreGauche >= 40 &&
            robotState.distanceTelemetreExtremeDroite >= 40 &&
            robotState.distanceTelemetreExtremeGauche >= 40) {//pas d'obstacle
        positionObstacle = PAS_D_OBSTACLE;
    }

    //     else if(robotState.distanceTelemetreDroit < 40 ||
    //            robotState.distanceTelemetreCentre < 40 ||
    //            robotState.distanceTelemetreGauche < 40 ||
    //            robotState.distanceTelemetreExtremeDroite < 40 ||
    //            robotState.distanceTelemetreExtremeGauche < 40){
    //         positionObstacle = OBSTACLE_A_DROITE;
    //     }

    //Détermination de l'état àvenir du robot
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //    if (positionObstacle == PAS_D_OBSTACLE) {
    //        nextStateRobot = STATE_AVANCE;
    //        vitessed = 25;
    //        vitesseg = 25;
    //    } else if (positionObstacle == OBSTACLE_A_DROITE) {
    //        nextStateRobot = STATE_TOURNE_GAUCHE;
    //
    //        if (robotState.distanceTelemetreGauche > 40) {
    //            robotState.distanceTelemetreGauche = 40;
    //        }
    //        if (robotState.distanceTelemetreDroit > 40) {
    //            robotState.distanceTelemetreDroit = 40;
    //        }
    //        // int distanceGauchevalue 
    //        vitessed = (robotState.distanceTelemetreGauche * 0.329 + 12.026);
    //        vitesseg = (robotState.distanceTelemetreDroit * 0.329 + 12.026);
    //    } else if (positionObstacle == OBSTACLE_A_GAUCHE) {
    //        nextStateRobot = STATE_TOURNE_DROITE;
    //        vitessed = 10;
    //        vitesseg = 15;
    //    } else if (positionObstacle == OBSTACLE_EN_FACE) {
    //        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    //        vitessed = 15;
    //        vitesseg = -15;
    //    } else if (positionObstacle == OBSTACLE_EXTREMEGAUCHE) {
    //        nextStateRobot = STATE_TOURNE_DROITE;
    //        vitessed = 5;
    //        vitesseg = 20;
    //    } else if (positionObstacle == OBSTACLE_EXTREMEDROITE) {
    //        nextStateRobot = STATE_TOURNE_GAUCHE;
    //        vitessed = 20;
    //        vitesseg = 5;
    //    } else if (positionObstacle == OBSTACLE_EN_FACE_D) {
    //        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
    //        vitessed = 15;
    //        vitesseg = -15;
    //    } else if (positionObstacle == OBSTACLE_EN_FACE_G) {
    //        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    //        vitessed = -15;
    //        vitesseg = 15;
    //    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////







    if (positionObstacle == PAS_D_OBSTACLE) {
        nextStateRobot = STATE_AVANCE;
        vitessed = 25;
        vitesseg = 25;
    } else if (positionObstacle == OBSTACLE_A_DROITE) {
        nextStateRobot = STATE_TOURNE_GAUCHE;
        //        vitessed = 15;
        //        vitesseg = 10;
        if (robotState.distanceTelemetreGauche > 40) {
            robotState.distanceTelemetreGauche = 40;
        }
        if (robotState.distanceTelemetreDroit > 40) {
            robotState.distanceTelemetreDroit = 40;
        }
        // int distanceGauchevalue 
        vitessed = (robotState.distanceTelemetreGauche * 0.329 + 12.026);
        vitesseg = (robotState.distanceTelemetreDroit * 0.329 + 12.026);
    } else if (positionObstacle == OBSTACLE_A_GAUCHE) {
        nextStateRobot = STATE_TOURNE_DROITE;
        vitessed = 10;
        vitesseg = 15;
    } else if (positionObstacle == OBSTACLE_EN_FACE) {
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        vitessed = 15;
        vitesseg = -15;
    } else if (positionObstacle == OBSTACLE_EXTREMEGAUCHE) {
        nextStateRobot = STATE_TOURNE_DROITE;
        if (robotState.distanceTelemetreExtremeGauche > 40) {
            robotState.distanceTelemetreExtremeGauche = 40;
        }
        if (robotState.distanceTelemetreExtremeDroite > 40) {
            robotState.distanceTelemetreExtremeDroite = 40;
        }
        vitessed = (robotState.distanceTelemetreExtremeGauche * 0.329 + 12.026);
        vitesseg = (robotState.distanceTelemetreExtremeDroite * 0.329 + 12.026);
    } else if (positionObstacle == OBSTACLE_EXTREMEDROITE) {
        nextStateRobot = STATE_TOURNE_GAUCHE;
        vitessed = 20;
        vitesseg = 5;
    } else if (positionObstacle == OBSTACLE_EN_FACE_D) {
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        vitessed = 15;
        vitesseg = -15;
    } else if (positionObstacle == OBSTACLE_EN_FACE_G) {
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        vitessed = -15;
        vitesseg = 15;
    }

    //Si l'on n'est pas dans la transition de l'étape en cours
    if (nextStateRobot != stateRobot - 1) {
        stateRobot = nextStateRobot;
    }
}
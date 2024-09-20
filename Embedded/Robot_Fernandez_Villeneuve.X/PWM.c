//Partie PWM
#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "PWM.h"
#include "Robot.h"
#include "Utilities.h"
#include "QEI.h"


#define PWMPER 24.0
#define SPEED_TO_PERCENT 55.0

void InitPWM(void) {
    PTCON2bits.PCLKDIV = 0b000; //Divide by 1
    PTPER = 100 * PWMPER; //ÈPriode en pourcentage
    //ÈRglage PWM moteur 1 sur hacheur 1
    IOCON1bits.PMOD = 0b11; //PWM I/O pin pair is in the True Independent Output mode
    IOCON1bits.PENL = 1;
    IOCON1bits.PENH = 1;
    FCLCON1 = 0x0003; //ÈDsactive la gestion des faults
    IOCON2bits.PMOD = 0b11; //PWM I/O pin pair is in the True Independent Output mode
    IOCON2bits.PENL = 1;
    IOCON2bits.PENH = 1;
    FCLCON2 = 0x0003; //ÈDsactive la gestion des faults
    /* Enable PWM Module */
    PTCONbits.PTEN = 1;
}

double talon = 20;

void PWMSetSpeed(float vitesseEnPourcents) {
    PDC1 = vitesseEnPourcents * PWMPER + talon;
    SDC1 = talon;
}

void PWMUpdateSpeed() {
    // Cette fonction est appelee sur timer et permet de suivre des rampes d acceleration
    if (robotState.vitesseDroiteCommandeCourante < robotState.vitesseDroiteConsigne)
        robotState.vitesseDroiteCommandeCourante = Min(
            robotState.vitesseDroiteCommandeCourante + acceleration,
            robotState.vitesseDroiteConsigne);
    if (robotState.vitesseDroiteCommandeCourante > robotState.vitesseDroiteConsigne)
        robotState.vitesseDroiteCommandeCourante = Max(
            robotState.vitesseDroiteCommandeCourante - acceleration,
            robotState.vitesseDroiteConsigne);
    if (robotState.vitesseDroiteCommandeCourante >= 0) {
        PDC1 = robotState.vitesseDroiteCommandeCourante * PWMPER + talon;
        SDC1 = talon;

    } else {
        PDC1 = talon;
        SDC1 = -robotState.vitesseDroiteCommandeCourante * PWMPER + talon;
    }
    if (robotState.vitesseGaucheCommandeCourante < robotState.vitesseGaucheConsigne)
        robotState.vitesseGaucheCommandeCourante = Min(
            robotState.vitesseGaucheCommandeCourante + acceleration,
            robotState.vitesseGaucheConsigne);
    if (robotState.vitesseGaucheCommandeCourante > robotState.vitesseGaucheConsigne)
        robotState.vitesseGaucheCommandeCourante = Max(
            robotState.vitesseGaucheCommandeCourante - acceleration,
            robotState.vitesseGaucheConsigne);
    if (robotState.vitesseGaucheCommandeCourante > 0) {
        PDC2 = robotState.vitesseGaucheCommandeCourante * PWMPER + talon;
        SDC2 = talon;
    } else {
        PDC2 = talon;
        SDC2 = -robotState.vitesseGaucheCommandeCourante * PWMPER + talon;
    }
}

//void PWMSetSpeed(float vitesseEnPourcents, int numero_moteur) {
//    robotState.vitesseGaucheCommandeCourante = vitesseEnPourcents;
//    if (numero_moteur == 1) {
//        if (vitesseEnPourcents < 0) {
//            MOTEUR_GAUCHE_L_PWM_ENABLE = 0; //Pilotage de la pin en mode IO
//            MOTEUR_GAUCHE_L_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
//            MOTEUR_GAUCHE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
//        }
//        else {
//            MOTEUR_GAUCHE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode IO
//            MOTEUR_GAUCHE_H_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
//            MOTEUR_GAUCHE_H_PWM_ENABLE = 0; //Pilotage de la pin en mode PWM
//        }
//        MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante * PWMPER);
//    } 
//    
//    else if (numero_moteur == 0){
//        if (vitesseEnPourcents < 0) {
//            MOTEUR_DROITE_L_PWM_ENABLE = 0; //Pilotage de la pin en mode IO
//            MOTEUR_DROITE_L_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
//            MOTEUR_DROITE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
//        }
//        else {
//            MOTEUR_DROITE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode IO
//            MOTEUR_DROITE_H_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
//            MOTEUR_DROITE_H_PWM_ENABLE = 0; //Pilotage de la pin en mode PWM
//        }
//        MOTEUR_DROITE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante * PWMPER);
//   }
//
// }

////void PWMUpdateSpeed() {
////    // Cette fonction est appelÈe sur timer et permet de suivre des rampes d?accÈlÈration
////    //    if (robotState.vitesseDroiteCommandeCourante < robotState.vitesseDroiteConsigne)
////    //        robotState.vitesseDroiteCommandeCourante = Min(
////    //            robotState.vitesseDroiteCommandeCourante + acceleration,
////    //            robotState.vitesseDroiteConsigne);
////    //    if (robotState.vitesseDroiteCommandeCourante > robotState.vitesseDroiteConsigne)
////    //        robotState.vitesseDroiteCommandeCourante = Max(
////    //            robotState.vitesseDroiteCommandeCourante - acceleration,
////    //            robotState.vitesseDroiteConsigne);
////
////    robotState.vitesseDroiteCommandeCourante = robotState.vitesseDroiteConsigne;
////
////    if (robotState.vitesseDroiteCommandeCourante > 0) {
////        MOTEUR_DROITE_H_PWM_ENABLE = 0; //pilotage de la pin en mode IO
////        MOTEUR_DROITE_H_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
////        MOTEUR_DROITE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
////    } else {
////        MOTEUR_DROITE_L_PWM_ENABLE = 0; //pilotage de la pin en mode IO
////        MOTEUR_DROITE_L_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
////        MOTEUR_DROITE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
////    }
////    MOTEUR_DROITE_DUTY_CYCLE = Abs(robotState.vitesseDroiteCommandeCourante) * PWMPER;
////
////    //    if (robotState.vitesseGaucheCommandeCourante < robotState.vitesseGaucheConsigne)
////    //        robotState.vitesseGaucheCommandeCourante = Min(
////    //            robotState.vitesseGaucheCommandeCourante + acceleration,
////    //            robotState.vitesseGaucheConsigne);
////    //    if (robotState.vitesseGaucheCommandeCourante > robotState.vitesseGaucheConsigne)
////    //        robotState.vitesseGaucheCommandeCourante = Max(
////    //            robotState.vitesseGaucheCommandeCourante - acceleration,
////    //            robotState.vitesseGaucheConsigne);
////
////    robotState.vitesseGaucheCommandeCourante = robotState.vitesseGaucheConsigne;
////
////
////    if (robotState.vitesseGaucheCommandeCourante > 0) {
////        MOTEUR_GAUCHE_L_PWM_ENABLE = 0; //pilotage de la pin en mode IO
////        MOTEUR_GAUCHE_L_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
////        MOTEUR_GAUCHE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
////    } else {
////        MOTEUR_GAUCHE_H_PWM_ENABLE = 0; //pilotage de la pin en mode IO
////        MOTEUR_GAUCHE_H_IO_OUTPUT = 1; //Mise ‡ 1 de la pin
////        MOTEUR_GAUCHE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
////    }
////    MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante) * PWMPER;
////}

void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur) {

    if (moteur == 0) {
        robotState.vitesseDroiteConsigne = vitesseEnPourcents;
    } else if (moteur == 1) {
        robotState.vitesseGaucheConsigne = -vitesseEnPourcents;
    }
}

void PWMSetSpeedConsigneLineaire(float vitesseEnMs, char moteur) {
    if (moteur == 0) {
        robotState.vitesseDroiteConsigne = vitesseEnMs * SPEED_TO_PERCENT;
    } else if (moteur == 1) {
        robotState.vitesseGaucheConsigne = vitesseEnMs * SPEED_TO_PERCENT;
    }
}

void PWMSetSpeedConsignePolaire(float xcorrection, float thetacorrection) {
    robotState.vitesseDroiteConsigne = SPEED_TO_PERCENT * (xcorrection + thetacorrection * DISTROUES / 2);
    robotState.vitesseGaucheConsigne = SPEED_TO_PERCENT * (xcorrection - thetacorrection * DISTROUES / 2);
}
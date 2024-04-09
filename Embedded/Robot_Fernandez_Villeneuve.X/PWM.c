//Partie PWM
#include <xc.h> // library xc.h inclut tous les uC
#include "IO.h"
#include "PWM.h"
#include "Robot.h"
#include "Utilities.h"
#include "QEI.h"


#define PWMPER 40.0
#define SPEED_TO_PERCENT 55.0


void InitPWM(void) {
    PTCON2bits.PCLKDIV = 0b000; //Divide by 1
    PTPER = 100 * PWMPER; //Période en pourcentage

    //Réglage PWM moteur 1 sur hacheur 1
    IOCON1bits.POLH = 1; //High = 1 and active on low =0
    IOCON1bits.POLL = 1; //High = 1
    IOCON1bits.PMOD = 0b01; //Set PWM Mode to Redundant
    FCLCON1 = 0x0003; //Désactive la gestion des faults

    //Reglage PWM moteur 2 sur hacheur 6
    IOCON6bits.POLH = 1; //High = 1
    IOCON6bits.POLL = 1; //High = 1
    IOCON6bits.PMOD = 0b01; //Set PWM Mode to Redundant
    FCLCON6 = 0x0003; //Désactive la gestion des faults

    /* Enable PWM Module */
    PTCONbits.PTEN = 1;
}

//void PWMSetSpeed(float vitesseEnPourcents, int numero_moteur) {
//    robotState.vitesseGaucheCommandeCourante = vitesseEnPourcents;
//    if (numero_moteur == 1) {
//        if (vitesseEnPourcents < 0) {
//            MOTEUR_GAUCHE_L_PWM_ENABLE = 0; //Pilotage de la pin en mode IO
//            MOTEUR_GAUCHE_L_IO_OUTPUT = 1; //Mise à 1 de la pin
//            MOTEUR_GAUCHE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
//        }
//        else {
//            MOTEUR_GAUCHE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode IO
//            MOTEUR_GAUCHE_H_IO_OUTPUT = 1; //Mise à 1 de la pin
//            MOTEUR_GAUCHE_H_PWM_ENABLE = 0; //Pilotage de la pin en mode PWM
//        }
//        MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante * PWMPER);
//    } 
//    
//    else if (numero_moteur == 0){
//        if (vitesseEnPourcents < 0) {
//            MOTEUR_DROITE_L_PWM_ENABLE = 0; //Pilotage de la pin en mode IO
//            MOTEUR_DROITE_L_IO_OUTPUT = 1; //Mise à 1 de la pin
//            MOTEUR_DROITE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
//        }
//        else {
//            MOTEUR_DROITE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode IO
//            MOTEUR_DROITE_H_IO_OUTPUT = 1; //Mise à 1 de la pin
//            MOTEUR_DROITE_H_PWM_ENABLE = 0; //Pilotage de la pin en mode PWM
//        }
//        MOTEUR_DROITE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante * PWMPER);
//   }
//
// }

void PWMUpdateSpeed() {
    // Cette fonction est appelée sur timer et permet de suivre des rampes d?accélération
//    if (robotState.vitesseDroiteCommandeCourante < robotState.vitesseDroiteConsigne)
//        robotState.vitesseDroiteCommandeCourante = Min(
//            robotState.vitesseDroiteCommandeCourante + acceleration,
//            robotState.vitesseDroiteConsigne);
//    if (robotState.vitesseDroiteCommandeCourante > robotState.vitesseDroiteConsigne)
//        robotState.vitesseDroiteCommandeCourante = Max(
//            robotState.vitesseDroiteCommandeCourante - acceleration,
//            robotState.vitesseDroiteConsigne);

    robotState.vitesseDroiteCommandeCourante = robotState.vitesseDroiteConsigne;
            
    if (robotState.vitesseDroiteCommandeCourante > 0) {
        MOTEUR_DROITE_H_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_DROITE_H_IO_OUTPUT = 1; //Mise à 1 de la pin
        MOTEUR_DROITE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    } else {
        MOTEUR_DROITE_L_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_DROITE_L_IO_OUTPUT = 1; //Mise à 1 de la pin
        MOTEUR_DROITE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    }
    MOTEUR_DROITE_DUTY_CYCLE = Abs(robotState.vitesseDroiteCommandeCourante) * PWMPER;

//    if (robotState.vitesseGaucheCommandeCourante < robotState.vitesseGaucheConsigne)
//        robotState.vitesseGaucheCommandeCourante = Min(
//            robotState.vitesseGaucheCommandeCourante + acceleration,
//            robotState.vitesseGaucheConsigne);
//    if (robotState.vitesseGaucheCommandeCourante > robotState.vitesseGaucheConsigne)
//        robotState.vitesseGaucheCommandeCourante = Max(
//            robotState.vitesseGaucheCommandeCourante - acceleration,
//            robotState.vitesseGaucheConsigne);
    
    robotState.vitesseGaucheCommandeCourante = robotState.vitesseGaucheConsigne;
    

    if (robotState.vitesseGaucheCommandeCourante > 0) {
        MOTEUR_GAUCHE_L_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_GAUCHE_L_IO_OUTPUT = 1; //Mise à 1 de la pin
        MOTEUR_GAUCHE_H_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    } else {
        MOTEUR_GAUCHE_H_PWM_ENABLE = 0; //pilotage de la pin en mode IO
        MOTEUR_GAUCHE_H_IO_OUTPUT = 1; //Mise à 1 de la pin
        MOTEUR_GAUCHE_L_PWM_ENABLE = 1; //Pilotage de la pin en mode PWM
    }
    MOTEUR_GAUCHE_DUTY_CYCLE = Abs(robotState.vitesseGaucheCommandeCourante) * PWMPER;
}

void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur) {

    if (moteur == 0) {
        robotState.vitesseDroiteConsigne = vitesseEnPourcents;
    }
    else if (moteur == 1) {
        robotState.vitesseGaucheConsigne = vitesseEnPourcents;
    }
}

void PWMSetSpeedConsigneLineaire(float vitesseEnMs, char moteur){
     if (moteur == 0) {
        robotState.vitesseDroiteConsigne = vitesseEnMs * SPEED_TO_PERCENT ;
    }
    else if (moteur == 1) {
        robotState.vitesseGaucheConsigne = vitesseEnMs * SPEED_TO_PERCENT ;
    }
}

void PWMSetSpeedConsignePolaire(float xcorrection, float thetacorrection){
    robotState.vitesseDroiteConsigne = SPEED_TO_PERCENT * (xcorrection + thetacorrection*DISTROUES/2);
    robotState.vitesseGaucheConsigne = SPEED_TO_PERCENT * (xcorrection - thetacorrection*DISTROUES/2);
}
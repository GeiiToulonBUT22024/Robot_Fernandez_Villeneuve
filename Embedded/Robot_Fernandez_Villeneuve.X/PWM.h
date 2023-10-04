/* 
 * File:   PWM.h
 * Author: GEII Robot
 *
 * Created on 27 septembre 2023, 08:48
 */

#ifndef PWM_H
#define	PWM_H
#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1

void InitPWM(void);
//void PWMSetSpeed(float vitesseEnPourcents, int numero_moteur);
void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur);
#endif	/* PWM_H */


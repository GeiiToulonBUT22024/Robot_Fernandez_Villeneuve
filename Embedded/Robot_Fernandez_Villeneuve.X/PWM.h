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
void PWMUpdateSpeed();
void PWMSetSpeedConsigne(float vitesseEnPourcents, char moteur);
void PWMSetSpeedConsignePolaire(float xcorrection, float thetacorrection);
void PWMSetSpeedConsigneLineaire(float vitesseEnMs, char moteur);

#define acceleration 2

#endif	/* PWM_H */
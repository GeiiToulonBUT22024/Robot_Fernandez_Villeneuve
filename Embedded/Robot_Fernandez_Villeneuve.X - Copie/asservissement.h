/* 
 * File:   asservissement.h
 * Author: GEII Robot
 *
 * Created on 14 février 2024, 08:57
 */

#ifndef ASSERVISSEMENT_H
#define	ASSERVISSEMENT_H

typedef struct _PidCorrector
{
    float Kp;
    float Ki;
    float Kd;
    float erreurProportionelleMax;
    float erreurIntegraleMax;
    float erreurDeriveeMax;
    float erreurIntegrale;
    float epsilon_1;
    float erreur;
    //For Debug only
    float corrP;
    float corrI;
    float corrD;
} PidCorrector;

void SetupPidAsservissement(volatile PidCorrector* PidCorr, float Kp, float Ki, float Kd, float proportionelleMax, float integralMax, float deriveeMax);
void SendPidInfo(void);
float Correcteur(volatile PidCorrector* PidCorr, float erreur);
void UpdateAsservissement();

#endif	/* ASSERVISSEMENT_H */


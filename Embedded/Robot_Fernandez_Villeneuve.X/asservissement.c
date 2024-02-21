#include "asservissement.h"
#include "Robot.h"
#include "Utilities.h"
#include "UART_Protocol.h"
#include "QEI.h"

void SetupPidAsservissement(volatile PidCorrector* PidCorr, float Kp, float Ki, float Kd, float proportionelleMax, float integralMax, float deriveeMax) {
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;
}

void SendPidInfo() {
    unsigned char tabParamX[24];
    getBytesFromFloat(tabParamX, 0, robotState.PidX.Kp);
    getBytesFromFloat(tabParamX, 4, robotState.PidX.Ki);
    getBytesFromFloat(tabParamX, 8, robotState.PidX.Kd);
    getBytesFromFloat(tabParamX, 12, robotState.PidX.erreurProportionelleMax);
    getBytesFromFloat(tabParamX, 16, robotState.PidX.erreurIntegraleMax);
    getBytesFromFloat(tabParamX, 20, robotState.PidX.erreurDeriveeMax);
    UartEncodeAndSendMessage(0x70, 24, tabParamX);

    unsigned char tabParamTheta[24];
    getBytesFromFloat(tabParamTheta, 0, robotState.PidTheta.Kp);
    getBytesFromFloat(tabParamTheta, 4, robotState.PidTheta.Ki);
    getBytesFromFloat(tabParamTheta, 8, robotState.PidTheta.Kd);
    getBytesFromFloat(tabParamTheta, 12, robotState.PidTheta.erreurProportionelleMax);
    getBytesFromFloat(tabParamTheta, 16, robotState.PidTheta.erreurIntegraleMax);
    getBytesFromFloat(tabParamTheta, 20, robotState.PidTheta.erreurDeriveeMax);
    UartEncodeAndSendMessage(0x71, 24, tabParamTheta);
}

float Correcteur(volatile PidCorrector* PidCorr, float erreur) {
    PidCorr->erreur = erreur;
    float erreurProportionnelle = LimitToInterval(erreur, -PidCorr->erreurProportionelleMax / PidCorr->Kp, PidCorr->erreurProportionelleMax / PidCorr->Kp);
    PidCorr->corrP = erreurProportionnelle * PidCorr->Kp;
    PidCorr->erreurIntegrale += erreur / FREQ_ECH_QEI;
    PidCorr->erreurIntegrale = LimitToInterval(erreur, -PidCorr->erreurIntegraleMax / PidCorr->Ki, PidCorr->erreurIntegraleMax / PidCorr->Ki);
    PidCorr->corrI = PidCorr->erreurIntegrale * PidCorr->Ki;
    float erreurDerivee = (erreur - PidCorr->epsilon_1) * FREQ_ECH_QEI;
    float deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax / PidCorr->Kd, PidCorr->erreurDeriveeMax / PidCorr->Kd);
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;

    return PidCorr->corrP + PidCorr->corrI + PidCorr->corrD;
}

void UpdateAsservissement() {
    robotState.PidX.erreur = ...;
    robotState.PidTheta.erreur = ...;
    robotState.xCorrectionVitessePourcent =
            Correcteur(&robotState.PidX, robotState.PidX.erreur);
    robotState.thetaCorrectionVitessePourcent = ...;
    PWMSetSpeedConsignePolaire(robotState.xCorrectionVitessePourcent,
            robotState.thetaCorrectionVitessePourcent);
}
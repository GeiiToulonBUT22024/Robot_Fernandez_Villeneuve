#include "asservissement.h"
#include "Robot.h"
#include "Utilities.h"
#include "UART_Protocol.h"
#include "QEI.h"
#include "PWM.h"

void SetupPidAsservissement(volatile PidCorrector* PidCorr, float Kp, float Ki, float Kd, float proportionelleMax, float integralMax, float deriveeMax) {
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;
}

float Correcteur(volatile PidCorrector* PidCorr, float erreur) {
    PidCorr->erreur = erreur;
    float erreurProportionnelle;
    if(PidCorr->Kp!=0)
        erreurProportionnelle = LimitToInterval(erreur, -PidCorr->erreurProportionelleMax / PidCorr->Kp, PidCorr->erreurProportionelleMax / PidCorr->Kp);
    else
        erreurProportionnelle = erreur;
    PidCorr->corrP = erreurProportionnelle * PidCorr->Kp;
    
    PidCorr->erreurIntegrale += erreur / FREQ_ECH_QEI;
    if(PidCorr->Ki!=0)
        PidCorr->erreurIntegrale = LimitToInterval(PidCorr->erreurIntegrale, -PidCorr->erreurIntegraleMax / PidCorr->Ki, PidCorr->erreurIntegraleMax / PidCorr->Ki);
    else
        PidCorr->erreurIntegrale = erreur;
    PidCorr->corrI = PidCorr->erreurIntegrale * PidCorr->Ki;
    float erreurDerivee = (erreur - PidCorr->epsilon_1) * FREQ_ECH_QEI;
       
    float deriveeBornee;
    if(PidCorr->Kd!=0)
        deriveeBornee = LimitToInterval(erreurDerivee, -PidCorr->erreurDeriveeMax / PidCorr->Kd, PidCorr->erreurDeriveeMax / PidCorr->Kd);
    else
        deriveeBornee = erreurDerivee;
    PidCorr->epsilon_1 = erreur;
    PidCorr->corrD = deriveeBornee * PidCorr->Kd;

    return PidCorr->corrP + PidCorr->corrI + PidCorr->corrD;
}

void UpdateAsservissement() {
    robotState.PidX.erreur = robotState.vitesseConsigneLineaire - robotState.vitesseLineaireFromOdometry;
    robotState.PidTheta.erreur = robotState.vitesseConsigneAngulaire - robotState.vitesseAngulaireFromOdometry;
    robotState.xCorrectionVitesse = Correcteur(&robotState.PidX, robotState.PidX.erreur);
    robotState.thetaCorrectionVitesse = Correcteur(&robotState.PidTheta, robotState.PidTheta.erreur);
    PWMSetSpeedConsignePolaire(robotState.xCorrectionVitesse, robotState.thetaCorrectionVitesse);
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

    unsigned char tabCorrecteur[32];
    getBytesFromFloat(tabCorrecteur, 0, robotState.PidX.corrP);
    getBytesFromFloat(tabCorrecteur, 4, robotState.PidX.corrI);
    getBytesFromFloat(tabCorrecteur, 8, robotState.PidX.corrD);
    getBytesFromFloat(tabCorrecteur, 12, robotState.PidTheta.corrP);
    getBytesFromFloat(tabCorrecteur, 16, robotState.PidTheta.corrI);
    getBytesFromFloat(tabCorrecteur, 20, robotState.PidTheta.corrD);
    getBytesFromFloat(tabCorrecteur, 24, robotState.PidX.erreur);
    getBytesFromFloat(tabCorrecteur, 28, robotState.PidTheta.erreur);
    UartEncodeAndSendMessage(0x72, 32, tabCorrecteur);

    unsigned char paramConsigne[8];
    getBytesFromFloat(paramConsigne, 0, robotState.vitesseConsigneLineaire);
    getBytesFromFloat(paramConsigne, 4, robotState.vitesseConsigneAngulaire);
    UartEncodeAndSendMessage(0x90, 8, paramConsigne);
    
    unsigned char paramCommande[8];
    getBytesFromFloat(paramCommande, 0, robotState.xCorrectionVitesse);
    getBytesFromFloat(paramCommande, 4, robotState.thetaCorrectionVitesse);
    UartEncodeAndSendMessage(0x91, 8, paramCommande);
}
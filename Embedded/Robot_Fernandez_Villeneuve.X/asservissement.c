#include "asservissement.h"
#include "Robot.h"

void SetupPidAsservissement(volatile PidCorrector* PidCorr, double Kp, double Ki, double Kd, double proportionelleMax, double integralMax, double deriveeMax) {
    PidCorr->Kp = Kp;
    PidCorr->erreurProportionelleMax = proportionelleMax; //On limite la correction due au Kp
    PidCorr->Ki = Ki;
    PidCorr->erreurIntegraleMax = integralMax; //On limite la correction due au Ki
    PidCorr->Kd = Kd;
    PidCorr->erreurDeriveeMax = deriveeMax;


    unsigned char tabParam[24];
    getBytesFromFloat(tabParam, 0, (float) (Kp));
    getBytesFromFloat(tabParam, 4, (float) (Ki));
    getBytesFromFloat(tabParam, 8, (float) (Kd));
    getBytesFromFloat(tabParam, 12, (float) (proportionelleMax));
    getBytesFromFloat(tabParam, 16, (float) (integralMax));
    getBytesFromFloat(tabParam, 20, (float) (deriveeMax));
    UartEncodeAndSendMessage(0x70, 24, tabParam);
}
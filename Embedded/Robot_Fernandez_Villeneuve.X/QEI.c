#include <math.h>
#include "QEI.h"
#include "Robot.h"
#include "Utilities.h"
#include "UART_Protocol.h"
#include "timer.h"
#include "IO.h"
#include "xc.h"

#define DISTROUES 281.2

void InitQEI1() {
    QEI1IOCbits.SWPAB = 1; //QEAx and QEBx are swapped
    QEI1GECL = 0xFFFF;
    QEI1GECH = 0xFFFF;
    QEI1CONbits.QEIEN = 1; // Enable QEI Module
}

void InitQEI2() {
    QEI2IOCbits.SWPAB = 1; //QEAx and QEBx are not swapped
    QEI2GECL = 0xFFFF;
    QEI2GECH = 0xFFFF;
    QEI2CONbits.QEIEN = 1; // Enable QEI Module
}

void QEIUpdateData() {
    //On sauvegarde les anciennes valeurs
    QeiDroitPosition_T_1 = QeiDroitPosition;
    QeiGauchePosition_T_1 = QeiGauchePosition;
    //On actualise les valeurs des positions
    long QEI1RawValue = POS1CNTL;
    QEI1RawValue += ((long) POS1HLD << 16);
    long QEI2RawValue = POS2CNTL;
    QEI2RawValue += ((long) POS2HLD << 16);
    //Conversion en mm (regle pour la taille des roues codeuses)
    QeiDroitPosition = 0.01620 * QEI1RawValue;
    QeiGauchePosition = -0.01620 * QEI2RawValue;
    //Calcul des deltas de position
    delta_d = QeiDroitPosition - QeiDroitPosition_T_1;
    delta_g = QeiGauchePosition - QeiGauchePosition_T_1;
    //Calcul des vitesses
    //attention a remultiplier par la frequence d echantillonnage
    robotState.vitesseDroitFromOdometry = delta_d*FREQ_ECH_QEI;
    robotState.vitesseGaucheFromOdometry = delta_g*FREQ_ECH_QEI;
    robotState.vitesseLineaireFromOdometry = (robotState.vitesseDroitFromOdometry + robotState.vitesseGaucheFromOdometry) / 2;
    robotState.vitesseAngulaireFromOdometry = (robotState.vitesseDroitFromOdometry - robotState.vitesseGaucheFromOdometry) / (DISTROUES);
    //Mise a jour du positionnement terrain a t-1
    robotState.xPosFromOdometry_1 = robotState.xPosFromOdometry;
    robotState.yPosFromOdometry_1 = robotState.yPosFromOdometry;
    robotState.angleRadianFromOdometry_1 = robotState.angleRadianFromOdometry;
    //Calcul des positions dans le referentiel du terrain
    robotState.xPosFromOdometry = robotState.xPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry * cos(robotState.angleRadianFromOdometry_1 + (robotState.vitesseAngulaireFromOdometry / (2 * FREQ_ECH_QEI))) / FREQ_ECH_QEI;
    robotState.yPosFromOdometry = robotState.yPosFromOdometry_1 + robotState.vitesseLineaireFromOdometry * sin(robotState.angleRadianFromOdometry_1 + (robotState.vitesseAngulaireFromOdometry / (2 * FREQ_ECH_QEI))) / FREQ_ECH_QEI;
    robotState.angleRadianFromOdometry = robotState.angleRadianFromOdometry_1 + robotState.vitesseAngulaireFromOdometry / FREQ_ECH_QEI;
    if (robotState.angleRadianFromOdometry > PI)
        robotState.angleRadianFromOdometry -= 2 * PI;
    if (robotState.angleRadianFromOdometry < -PI)
        robotState.angleRadianFromOdometry += 2 * PI;
}

#define POSITION_DATA 0x0061

void SendPositionData() {
    unsigned char positionPayload[32];
    getBytesFromInt32(positionPayload, 0, timestamp);
    getBytesFromFloat(positionPayload, 4, (float) (robotState.xPosFromOdometry));
    getBytesFromFloat(positionPayload, 8, (float) (robotState.yPosFromOdometry));
    getBytesFromFloat(positionPayload, 12, (float) (robotState.angleRadianFromOdometry));
    getBytesFromFloat(positionPayload, 16, (float) (robotState.vitesseLineaireFromOdometry));
    getBytesFromFloat(positionPayload, 20, (float) (robotState.vitesseAngulaireFromOdometry));
    getBytesFromFloat(positionPayload, 24, (float) (robotState.vitesseGaucheFromOdometry));
    getBytesFromFloat(positionPayload, 28, (float) (robotState.vitesseDroitFromOdometry));
    UartEncodeAndSendMessage(POSITION_DATA, 32, positionPayload);
}
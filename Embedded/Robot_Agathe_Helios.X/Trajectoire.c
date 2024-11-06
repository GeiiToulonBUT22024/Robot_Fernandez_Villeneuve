#include "Trajectoire.h"
#include "Utilities.h"
#include "Robot.h"
#include "math.h"
#include "PWM.h"
#include "UART_Protocol.h"
#include "timer.h"
#include "ChipConfig.h"
#include "QEI.h"


volatile GhostPosition ghostPosition;
static unsigned long lastUpdateTime = 0;
int i = 0;

struct Square {
    double x;
    double y;
};
typedef struct Square square;

square point_carree[7] = {
    {0, 0},
    {0, 0.5},
    {-1, 0.5},
    {-1, -0.5},
    {0, -0.5},
    {0, 0},
    {1.3, 0}
};

void InitTrajectory(void) {
    ghostPosition.posX = 1.33;
    ghostPosition.posY = 0.0;
    ghostPosition.thetaGhost = -PI;
    ghostPosition.vitesseLineaire = 0.0;
    ghostPosition.vitesseAngulaire = 0.0;
    ghostPosition.waypointX = 0.0;
    ghostPosition.waypointY = 0.0;
    ghostPosition.thetaWaypoint = 0.0;
    ghostPosition.distanceRestante = 0.0;
    ghostPosition.last_waypointX = 100.0;
    ghostPosition.last_waypointY = 100.0;

    ghostPosition.state = IDLE;
}

void UpdateTrajectory() {
   
    ghostPosition.thetaWaypoint = atan2((ghostPosition.waypointY - ghostPosition.posY), (ghostPosition.waypointX - ghostPosition.posX));
    ghostPosition.thetaRestant = ModuloByAngle(ghostPosition.thetaGhost, ghostPosition.thetaWaypoint) - ghostPosition.thetaGhost;
    ghostPosition.thetaArret = ghostPosition.vitesseAngulaire * ghostPosition.vitesseAngulaire / (2 * accelerationAngulaire);
    ghostPosition.incrementeAngulaire = ghostPosition.vitesseAngulaire / FREQ_T1;

    ghostPosition.distanceRestante = sqrt((ghostPosition.waypointY - ghostPosition.posY) * (ghostPosition.waypointY - ghostPosition.posY) + (ghostPosition.waypointX - ghostPosition.posX) * (ghostPosition.waypointX - ghostPosition.posX));
    ghostPosition.distanceArret = ghostPosition.vitesseLineaire * ghostPosition.vitesseLineaire / (2 * accelerationLineaire);
    ghostPosition.incrementeLineaire = ghostPosition.vitesseLineaire / FREQ_T1;

    switch (ghostPosition.state) {
        case IDLE:
            ghostPosition.vitesseLineaire = 0;
            ghostPosition.vitesseAngulaire = 0;
            //            if (ghostPosition.last_waypointX != ghostPosition.waypointX || ghostPosition.last_waypointY != ghostPosition.waypointY) {
            //                ghostPosition.state = ROTATION;
            //            }
            if (i < 7) {
                square next_point = point_carree[i++];
                ghostPosition.waypointX = next_point.x;
                ghostPosition.waypointY = next_point.y;
                ghostPosition.state = ROTATION;
            }
            break;

        case ROTATION:

            if (ghostPosition.vitesseAngulaire < 0) {
                ghostPosition.thetaArret = -ghostPosition.thetaArret;
            }

            if (((ghostPosition.thetaArret >= 0 && ghostPosition.thetaRestant >= 0) || (ghostPosition.thetaArret <= 0 && ghostPosition.thetaRestant <= 0))
                    && Abs(ghostPosition.thetaRestant) >= Abs(ghostPosition.thetaArret)) {

                if (ghostPosition.thetaRestant > 0) {
                    ghostPosition.vitesseAngulaire = Min(ghostPosition.vitesseAngulaire + accelerationAngulaire / FREQ_T1, VitesseMaxAngulaire);
                } else if (ghostPosition.thetaRestant < 0) {
                    ghostPosition.vitesseAngulaire = Max(ghostPosition.vitesseAngulaire - accelerationAngulaire / FREQ_T1, -VitesseMaxAngulaire);
                }
            } else {
                if (ghostPosition.thetaRestant >= 0 && ghostPosition.vitesseAngulaire > 0) {
                    ghostPosition.vitesseAngulaire = Max(ghostPosition.vitesseAngulaire - accelerationAngulaire / FREQ_T1, 0);
                } else if (ghostPosition.thetaRestant >= 0 && ghostPosition.vitesseAngulaire < 0) {
                    ghostPosition.vitesseAngulaire = Min(ghostPosition.vitesseAngulaire + accelerationAngulaire / FREQ_T1, 0);
                } else if (ghostPosition.thetaRestant <= 0 && ghostPosition.vitesseAngulaire > 0) {
                    ghostPosition.vitesseAngulaire = Max(ghostPosition.vitesseAngulaire - accelerationAngulaire / FREQ_T1, 0);
                } else if (ghostPosition.thetaRestant <= 0 && ghostPosition.vitesseAngulaire < 0) {
                    ghostPosition.vitesseAngulaire = Min(ghostPosition.vitesseAngulaire + accelerationAngulaire / FREQ_T1, 0);
                }

                if (Abs(ghostPosition.thetaRestant) < Abs(ghostPosition.incrementeAngulaire)) {
                    ghostPosition.incrementeAngulaire = ghostPosition.thetaRestant;
                }
            }

            ghostPosition.thetaGhost = ghostPosition.thetaGhost + ghostPosition.incrementeAngulaire;
            robotState.vitesseConsigneAngulaire = ghostPosition.vitesseAngulaire;

            if (ghostPosition.vitesseAngulaire == 0 && (Abs(ghostPosition.thetaRestant) < 0.01)) {
                ghostPosition.thetaGhost = ghostPosition.thetaWaypoint;
                if (i == 7) {
                    ghostPosition.state = IDLE;
                } else {
                    ghostPosition.state = DEPLACEMENTLINEAIRE;
                }
            }
            break;

        case DEPLACEMENTLINEAIRE:

            if ((ghostPosition.distanceRestante != 0) && (Modulo2PIAngleRadian(ghostPosition.thetaRestant) < 0.01)) {
                if (((ghostPosition.distanceArret >= 0 && ghostPosition.distanceRestante >= 0) || (ghostPosition.distanceArret <= 0 && ghostPosition.distanceRestante <= 0))
                        && Abs(ghostPosition.distanceRestante) >= Abs(ghostPosition.distanceArret)) {
                    if (ghostPosition.distanceRestante > 0) {
                        ghostPosition.vitesseLineaire = Min(ghostPosition.vitesseLineaire + accelerationLineaire / FREQ_T1, VitesseMaxLineaire);
                    } else if (ghostPosition.distanceRestante < 0) {
                        ghostPosition.vitesseLineaire = Max(ghostPosition.vitesseLineaire - accelerationLineaire / FREQ_T1, -VitesseMaxLineaire);
                    }
                } else {
                    if (ghostPosition.distanceRestante >= 0 && ghostPosition.vitesseLineaire > 0) {
                        ghostPosition.vitesseLineaire = Max(ghostPosition.vitesseLineaire - accelerationLineaire / FREQ_T1, 0);
                    } else if (ghostPosition.distanceRestante >= 0 && ghostPosition.vitesseLineaire < 0) {
                        ghostPosition.vitesseLineaire = Min(ghostPosition.vitesseLineaire + accelerationLineaire / FREQ_T1, 0);
                    } else if (ghostPosition.distanceRestante <= 0 && ghostPosition.vitesseLineaire > 0) {
                        ghostPosition.vitesseLineaire = Max(ghostPosition.vitesseLineaire - accelerationLineaire / FREQ_T1, 0);
                    } else if (ghostPosition.distanceRestante <= 0 && ghostPosition.vitesseLineaire < 0) {
                        ghostPosition.vitesseLineaire = Min(ghostPosition.vitesseLineaire + accelerationLineaire / FREQ_T1, 0);
                    }

                    if (Abs(ghostPosition.distanceRestante) < Abs(ghostPosition.incrementeLineaire)) {
                        ghostPosition.incrementeLineaire = ghostPosition.distanceRestante;

                    }
                }
            }
            if ((Abs(ghostPosition.distanceRestante) < 0.01)) {
                ghostPosition.vitesseLineaire = 0;
                ghostPosition.posX = ghostPosition.waypointX;
                ghostPosition.posY = ghostPosition.waypointY;
                ghostPosition.state = IDLE;
            }

            ghostPosition.posX += ghostPosition.incrementeLineaire * cos(ghostPosition.thetaGhost);
            ghostPosition.posY += ghostPosition.incrementeLineaire * sin(ghostPosition.thetaGhost);
            robotState.vitesseConsigneLineaire = ghostPosition.vitesseLineaire;

            break;
    }
}


void SendGhostInfo() {

    unsigned char tabGhost[24];
    getBytesFromFloat(tabGhost, 0, ghostPosition.posX);
    getBytesFromFloat(tabGhost, 4, ghostPosition.posY);
    getBytesFromFloat(tabGhost, 8, ghostPosition.thetaGhost);
    getBytesFromFloat(tabGhost, 12, ghostPosition.vitesseLineaire);
    getBytesFromFloat(tabGhost, 16, ghostPosition.vitesseAngulaire);
    getBytesFromFloat(tabGhost, 20, timestamp);
    UartEncodeAndSendMessage(0x50, 24, tabGhost);
}
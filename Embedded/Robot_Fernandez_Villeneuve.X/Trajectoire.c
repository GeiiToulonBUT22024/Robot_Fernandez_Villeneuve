#include "Trajectoire.h"
#include "Utilities.h"
#include "Robot.h"
#include "math.h"
#include "PWM.h"
#include "UART_Protocol.h"

volatile GhostPosition ghostPosition;

void orientation() {
    switch (ghostPosition.state) {
        case IDLE:
            ghostPosition.vitesseLineaire = 0;
            ghostPosition.vitesseAngulaire = 0;
            break;

        case ROTATION:
            ghostPosition.thetaRestant = ghostPosition.thetaWaypoint - ModuloByAngle(ghostPosition.thetaWaypoint, ghostPosition.thetaRobot);
            ghostPosition.thetaArret = pow(ghostPosition.vitesseAngulaire, 2) / 2 * ghostPosition.accelerationAngulaire;
            if (ghostPosition.thetaRestant > 0) {
                if (ghostPosition.vitesseAngulaire < 0) {
                    ghostPosition.vitesseAngulaire -= ghostPosition.accelerationAngulaire;
                } else {
                    if (ghostPosition.thetaRestant > ghostPosition.thetaArret) {
                        if (ghostPosition.vitesseAngulaire < VitesseMaxAngulaire) {
                            ghostPosition.vitesseAngulaire += ghostPosition.accelerationAngulaire;
                        } else {
                            ghostPosition.vitesseAngulaire = VitesseMaxAngulaire;
                        }
                    } else {
                        ghostPosition.vitesseAngulaire -= ghostPosition.accelerationAngulaire;
                    }
                }
            } else {
                if (ghostPosition.vitesseAngulaire > 0) {
                    ghostPosition.vitesseAngulaire -= ghostPosition.accelerationAngulaire;
                } else {
                    if (Abs(ghostPosition.thetaRestant) > ghostPosition.thetaArret) {
                        if (ghostPosition.vitesseAngulaire>-VitesseMaxAngulaire) {
                            ghostPosition.vitesseAngulaire -= ghostPosition.accelerationAngulaire;
                        } else {
                            ghostPosition.vitesseAngulaire = VitesseMaxAngulaire;
                        }
                    } else {
                        ghostPosition.vitesseAngulaire += ghostPosition.accelerationAngulaire;
                    }
                }
            }
            if (Abs(ghostPosition.thetaRestant) < ToleranceAngulaire) {
                ghostPosition.vitesseAngulaire = 0;
                ghostPosition.thetaRobot = ghostPosition.thetaWaypoint;
                ghostPosition.state = DEPLACEMENTLINEAIRE;
            }
            break;
        case DEPLACEMENTLINEAIRE:

            break;
    }
}

void SendGhostInfo() {

    unsigned char tabGhost[12];
    getBytesFromFloat(tabGhost, 0, ghostPosition.posX);
    getBytesFromFloat(tabGhost, 4, ghostPosition.posY);
    getBytesFromFloat(tabGhost, 8, ghostPosition.thetaRobot);
    UartEncodeAndSendMessage(0x50, 12, tabGhost);
}

#include "Trajectoire.h"
#include "Utilities.h"
#include "Robot.h"
#include "math.h"
#include "PWM.h"
#include "UART_Protocol.h"
#include "timer.h"

volatile GhostPosition ghostPosition;
static unsigned long lastUpdateTime = 0;

void InitTrajectory(void) {
    ghostPosition.posX = 0.0;
    ghostPosition.posY = 0.0;
    ghostPosition.thetaRobot = 0.0;
    ghostPosition.vitesseLineaire = 0.0;
    ghostPosition.vitesseAngulaire = 0.0;
    ghostPosition.waypointX = 0.0;
    ghostPosition.waypointY = 0.0;
    ghostPosition.thetaWaypoint = 0.0;
    ghostPosition.distanceToTarget = 0.0;
    ghostPosition.last_waypointX = 100.0;
    ghostPosition.last_waypointY = 100.0;

    ghostPosition.state = IDLE;
}

void UpdateTrajectory() {
//    switch (ghostPosition.state) {
//        case IDLE:
//            ghostPosition.vitesseLineaire = 0;
//            ghostPosition.vitesseAngulaire = 0;
//            if (ghostPosition.last_waypointX != ghostPosition.waypointX || ghostPosition.last_waypointY != ghostPosition.waypointY) {
//                ghostPosition.state = ROTATION;
//            }
//            break;
//
//        case ROTATION:
//            ghostPosition.target_angle = atan2((ghostPosition.waypointY - ghostPosition.posY), (ghostPosition.waypointX - ghostPosition.posX));
//            ghostPosition.thetaRestant = ghostPosition.thetaWaypoint - ModuloByAngle(ghostPosition.target_angle, ghostPosition.thetaRobot);
//            ghostPosition.thetaArret = pow(ghostPosition.vitesseAngulaire, 2) / (2 * accelerationAngulaire);
//
//            if (ghostPosition.thetaRestant > 0) {
//                if (ghostPosition.vitesseAngulaire < 0) {
//                    ghostPosition.vitesseAngulaire -= accelerationAngulaire;
//                } else {
//                    if (ghostPosition.thetaRestant > ghostPosition.thetaArret) {
//                        if (ghostPosition.vitesseAngulaire < VitesseMaxAngulaire) {
//                            ghostPosition.vitesseAngulaire += accelerationAngulaire;
//                        } else {
//                            ghostPosition.vitesseAngulaire = VitesseMaxAngulaire;
//                        }
//                    } else {
//                        ghostPosition.vitesseAngulaire -= accelerationAngulaire;
//                    }
//                }
//            } else {
//                if (ghostPosition.vitesseAngulaire > 0) {
//                    ghostPosition.vitesseAngulaire -= accelerationAngulaire;
//                } else {
//                    if (Abs(ghostPosition.thetaRestant) > ghostPosition.thetaArret) {
//                        if (ghostPosition.vitesseAngulaire>-VitesseMaxAngulaire) {
//                            ghostPosition.vitesseAngulaire -= accelerationAngulaire;
//                        } else {
//                            ghostPosition.vitesseAngulaire = VitesseMaxAngulaire;
//                        }
//                    } else {
//                        ghostPosition.vitesseAngulaire += accelerationAngulaire;
//                    }
//                }
//            }
//            if (Abs(ghostPosition.thetaRestant) < ToleranceAngulaire) {
//                ghostPosition.vitesseAngulaire = 0;
//                ghostPosition.thetaRobot = ghostPosition.thetaWaypoint;
//                ghostPosition.state = DEPLACEMENTLINEAIRE;
//            }
//            break;
//        case DEPLACEMENTLINEAIRE:
//            ghostPosition.distance = sqrt(pow((ghostPosition.waypointY - ghostPosition.posY), 2) + pow((ghostPosition.waypointX - ghostPosition.posX), 2));
//
//            if (ghostPosition.distance < Tolerancedistance) {
//                ghostPosition.state = IDLE;
//                ghostPosition.vitesseLineaire = 0.0;
//                ghostPosition.waypointX = ghostPosition.posX;
//                ghostPosition.waypointY = ghostPosition.posY;
//                return;
//            }
//            ghostPosition.distanceToTarget = ghostPosition.distance;
//
//            double distanceDecel = pow(ghostPosition.vitesseLineaire, 2) / (2 * accelerationLineaire);
//            double distanceAccel = (pow(VitesseMaxLineaire, 2) - pow(ghostPosition.vitesseLineaire, 2)) / (2 * accelerationLineaire);
//            double deltaTime = timestamp - lastUpdateTime;
//
//            if (ghostPosition.distance <= (distanceDecel + Tolerancedistance)) {
//                ghostPosition.vitesseLineaire -= accelerationLineaire * deltaTime;
//                ghostPosition.vitesseLineaire = fmax(ghostPosition.vitesseLineaire, 0);
//            } else if (ghostPosition.distance > distanceDecel + distanceAccel) {
//                ghostPosition.vitesseLineaire += accelerationLineaire * deltaTime;
//                ghostPosition.vitesseLineaire = fmin(ghostPosition.vitesseLineaire, VitesseMaxLineaire);
//            } else {
//                double vMedian = sqrt(VitesseMaxLineaire * ghostPosition.distance + ghostPosition.vitesseLineaire / 2);
//                ghostPosition.vitesseLineaire += accelerationLineaire * deltaTime;
//                ghostPosition.vitesseLineaire = fmin(ghostPosition.vitesseLineaire, vMedian);
//            }
//            ghostPosition.posX += ghostPosition.vitesseLineaire * cos(ghostPosition.thetaRobot) * deltaTime;
//            ghostPosition.posY += ghostPosition.vitesseLineaire * sin(ghostPosition.thetaRobot) * deltaTime;
//            break;
//
//            lastUpdateTime = timestamp;
//    }



        if (ghostPosition.last_waypointX != ghostPosition.waypointX || ghostPosition.last_waypointY != ghostPosition.waypointY) {
    
            ghostPosition.last_waypointX = ghostPosition.waypointX;
            ghostPosition.last_waypointY = ghostPosition.waypointY;
    
            ghostPosition.target_angle = atan2((ghostPosition.waypointY - ghostPosition.posY), (ghostPosition.waypointX - ghostPosition.posX));
            ghostPosition.thetaRestant = ghostPosition.thetaWaypoint - ModuloByAngle(ghostPosition.target_angle, ghostPosition.thetaRobot);
            ghostPosition.thetaArret = pow(ghostPosition.vitesseAngulaire, 2) / (2 * accelerationAngulaire);
    
            ghostPosition.distance = sqrt(pow((ghostPosition.waypointY - ghostPosition.posY), 2) + pow((ghostPosition.waypointX - ghostPosition.posX), 2));
    
            double distanceDecel = pow(ghostPosition.vitesseLineaire, 2) / (2 * accelerationLineaire);
            double distanceAccel = (pow(VitesseMaxLineaire, 2) - pow(ghostPosition.vitesseLineaire, 2)) / (2 * accelerationLineaire);
            double deltaTime = timestamp - lastUpdateTime;
    
            if (ghostPosition.thetaRestant > 0) {
                if (ghostPosition.vitesseAngulaire < 0) {
                    ghostPosition.vitesseAngulaire -= accelerationAngulaire;
                } else {
                    if (ghostPosition.thetaRestant > ghostPosition.thetaArret) {
                        if (ghostPosition.vitesseAngulaire < VitesseMaxAngulaire) {
                            ghostPosition.vitesseAngulaire += accelerationAngulaire;
                        } else {
                            ghostPosition.vitesseAngulaire = VitesseMaxAngulaire;
                        }
                    } else {
                        ghostPosition.vitesseAngulaire -= accelerationAngulaire;
                    }
                }
            } else {
                if (ghostPosition.vitesseAngulaire > 0) {
                    ghostPosition.vitesseAngulaire -= accelerationAngulaire;
                } else {
                    if (Abs(ghostPosition.thetaRestant) > ghostPosition.thetaArret) {
                        if (ghostPosition.vitesseAngulaire>-VitesseMaxAngulaire) {
                            ghostPosition.vitesseAngulaire -= accelerationAngulaire;
                        } else {
                            ghostPosition.vitesseAngulaire = VitesseMaxAngulaire;
                        }
                    } else {
                        ghostPosition.vitesseAngulaire += accelerationAngulaire;
                    }
                }
            }
            if (Abs(ghostPosition.thetaRestant) < ToleranceAngulaire) {
                ghostPosition.vitesseAngulaire = 0;
                ghostPosition.thetaRobot = ghostPosition.thetaWaypoint;
            }
    
            if (ghostPosition.distance < Tolerancedistance) {
                ghostPosition.vitesseLineaire = 0.0;
                ghostPosition.waypointX = ghostPosition.posX;
                ghostPosition.waypointY = ghostPosition.posY;
            }
    
            if (ghostPosition.distance <= (distanceDecel + Tolerancedistance)) {
                ghostPosition.vitesseLineaire -= accelerationLineaire * deltaTime;
                ghostPosition.vitesseLineaire = fmax(ghostPosition.vitesseLineaire, 0);
            } else if (ghostPosition.distance > distanceDecel + distanceAccel) {
                ghostPosition.vitesseLineaire += accelerationLineaire * deltaTime;
                ghostPosition.vitesseLineaire = fmin(ghostPosition.vitesseLineaire, VitesseMaxLineaire);
            } else {
                double vMedian = sqrt(VitesseMaxLineaire * ghostPosition.distance + ghostPosition.vitesseLineaire / 2);
                ghostPosition.vitesseLineaire += accelerationLineaire * deltaTime;
                ghostPosition.vitesseLineaire = fmin(ghostPosition.vitesseLineaire, vMedian);
            }
            ghostPosition.posX += ghostPosition.vitesseLineaire * cos(ghostPosition.thetaRobot) * deltaTime;
            ghostPosition.posY += ghostPosition.vitesseLineaire * sin(ghostPosition.thetaRobot) * deltaTime;
            
    
            lastUpdateTime = timestamp;
    
        }
        else {
            ghostPosition.vitesseLineaire = 0;
            ghostPosition.vitesseAngulaire = 0;
        }
}

void SendGhostInfo() {

    unsigned char tabGhost[24];
    getBytesFromFloat(tabGhost, 0, ghostPosition.posX);
    getBytesFromFloat(tabGhost, 4, ghostPosition.posY);
    getBytesFromFloat(tabGhost, 8, ghostPosition.thetaRobot);
    getBytesFromFloat(tabGhost, 12, ghostPosition.vitesseLineaire);
    getBytesFromFloat(tabGhost, 16, ghostPosition.vitesseAngulaire);
    getBytesFromFloat(tabGhost, 20, timestamp);
    UartEncodeAndSendMessage(0x50, 24, tabGhost);
}
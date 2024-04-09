/* 
 * File:   Trajectoire.h
 * Author: GEII Robot
 *
 * Created on 2 avril 2024, 14:00
 */

#ifndef TRAJECTOIRE_H
#define	TRAJECTOIRE_H

#define VitesseMaxAngulaire 3
#define ToleranceAngulaire 0.5

typedef enum {
    IDLE,
    ROTATION,
    DEPLACEMENTLINEAIRE,
} EtatTraj;

typedef struct {
    float thetaWaypoint;
    float thetaRobot;
    float thetaRestant;
    float thetaArret;
    float accelerationAngulaire;
    float vitesseLineaire;
    float vitesseAngulaire;
    float posX;
    float posY;

    EtatTraj state;

} GhostPosition;
extern volatile GhostPosition ghostPosition;

void orientation();
void SendGhostInfo();

#endif	/* TRAJECTOIRE_H */


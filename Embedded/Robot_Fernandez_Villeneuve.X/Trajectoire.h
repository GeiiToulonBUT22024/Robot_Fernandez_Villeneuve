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
#define accelerationAngulaire 0.01
#define VitesseMaxLineaire 1 // 1m.s-1
#define Tolerancedistance 0.1 // 10 cm
#define accelerationLineaire 0.1


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
    float vitesseLineaire;
    float vitesseAngulaire;
    float posX;
    float posY;
    float waypointX;
    float waypointY;
    float distanceToTarget;
    float target_angle;
    double distance;
    //float waypointX_projette;
    //float waypointY_projette; 

    EtatTraj state;

} GhostPosition;
extern volatile GhostPosition ghostPosition;

void InitTrajectory(void);
void UpdateTrajectory();
void SendGhostInfo();

#endif	/* TRAJECTOIRE_H */


/* 
 * File:   Trajectoire.h
 * Author: GEII Robot
 *
 * Created on 2 avril 2024, 14:00
 */

#ifndef TRAJECTOIRE_H
#define	TRAJECTOIRE_H


#define VitesseMaxAngulaire 0.2
#define ToleranceAngulaire 0.005
#define accelerationAngulaire 0.2
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
    float thetaGhost;
    float thetaRestant;
    float thetaArret;
    float incrementeAngulaire;
    float incrementeLineaire;
    float vitesseLineaire;
    float vitesseAngulaire;
    float posX;
    float posY;
    float waypoint;
    float waypointX;
    float waypointY;
    float distanceRestante;
    float target_angle;
    double distance;
    double distanceArret;
    float last_waypointX;
    float last_waypointY; 
    float erreurAngulaire;
    float erreurLineaire;

    EtatTraj state;

} GhostPosition;

extern volatile GhostPosition ghostPosition;

void InitTrajectory(void);
void UpdateTrajectory();
void SendGhostInfo();
void PIDPosition();

#endif	/* TRAJECTOIRE_H */


#ifndef ROBOT_H
#define ROBOT_H
#include "asservissement.h"

typedef struct robotStateBITS {

    union {

        struct {
            unsigned char taskEnCours;
            float vitesseGaucheConsigne;
            float vitesseGaucheCommandeCourante;
            float vitesseDroiteConsigne;
            float vitesseDroiteCommandeCourante;
            float distanceTelemetreDroit;
            float distanceTelemetreCentre;
            float distanceTelemetreGauche;
            float distanceTelemetreExtremeDroite;
            float distanceTelemetreExtremeGauche;

            double vitesseDroitFromOdometry;
            double vitesseGaucheFromOdometry;
            double vitesseLineaireFromOdometry;
            double vitesseAngulaireFromOdometry;
            double xPosFromOdometry_1;
            double yPosFromOdometry_1;
            double angleRadianFromOdometry_1;
            double xPosFromOdometry;
            double yPosFromOdometry;
            double angleRadianFromOdometry;

            //            double vitesseLineaireConsigne;
            //            double vitesseAngulaireConsigne;

            float xCorrectionVitesse;
            float thetaCorrectionVitesse;

            float vitesseConsigneLineaire;
            float vitesseConsigneAngulaire;


            PidCorrector PidX;
            PidCorrector PidTheta;

            int mode;
            
        };
    };
} ROBOT_STATE_BITS;
extern volatile ROBOT_STATE_BITS robotState;

void PWMUpdateSpeed();

#endif /* ROBOT_H */

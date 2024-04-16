using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InterfaceRobot
{
    public class Robot
    {
        public string receivedText = "";
        public float distanceTelemetreDroit;
        public float distanceTelemetreCentre;
        public float distanceTelemetreGauche;
        public float positionX0do;
        public float positionY0do;
        public float vitesseLineaireFromOdometry;
        public float vitesseAngulaireFromOdometry;
        public float vitesseGaucheFromOdometry;
        public float vitesseDroitFromOdometry;
        public float timestamp;
        public float Kp;
        public float Ki;
        public float Kd;
        public float limP;
        public float limI;
        public float limD;

        public float KpT;
        public float KiT;
        public float KdT;
        public float limPT;
        public float limIT;
        public float limDT;

        public float corrXP;
        public float corrXI;
        public float corrXD;
        public float corrTP;
        public float corrTI;
        public float corrTD;

        public float erreurX;
        public float erreurT;

        public float ConsigneLineaire;
        public float ConsigneAngulaire;

        public float CommandeLineaire;
        public float CommandeAngulaire;



        //public Queue<string> TextToDisplay = new Queue<string>();
        public Robot()
        {

        }
    }
    public class Ghost
    {
        public float posX;
        public float posY;
        public float thetaRobot;
        public float vitL;
        public float vitA;
        public float timestamp;
        public float objectif;
    }
}

﻿using System;
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

        public Queue<byte> byteListReceived = new Queue<byte>();
        public Robot()
        {

        }
    }
}

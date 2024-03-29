﻿using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort;
using System.Windows.Threading;
using Constants;
using System.Collections;
using System.Globalization;
using static System.Net.Mime.MediaTypeNames;

namespace InterfaceRobot
{
    /// <summary>
    /// Logique d'interaction pour MainWindow.xaml
    /// </summary>


    public partial class MainWindow : Window
    {
        Robot robot = new Robot();
        ReliableSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        byte[] byteList = new byte[20];



        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM8", 115200, Parity.None, 8, StopBits.One);
            serialPort1.OnDataReceivedEvent += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 50);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            oscilloPos.AddOrUpdateLine(0, 200, "Postion");
            oscilloPos.ChangeLineColor(0, Colors.DarkCyan);
            oscilloPos.isDisplayActivated = true;

            oscilloSpeed.AddOrUpdateLine(0, 200, "Speed");
            oscilloSpeed.ChangeLineColor(0, Colors.HotPink);
            oscilloSpeed.isDisplayActivated = true;

        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            # region old code 
            //if (robot.receivedText != "" && robot.receivedText != "\r\n")
            //{
            //    textBoxReception.Text += robot.receivedText;
            //    robot.receivedText = "";
            //}
            # endregion
            while (robot.TextToDisplay.Count != 0)
            {

                textBoxReception.Text +=  robot.TextToDisplay.Dequeue();
                ////textBoxReception.Text += "0x" + b.ToString("X2") + " ";
                //textBoxReception.Text += Convert.ToChar(b);
                //DecodeMessage(b);
            }
            asservSpeedDisplay.UpdateDisplay();
            asservSpeedDisplay.UpdateIndependantSpeedCommandValues(0, 0);
           
            asservSpeedDisplay.UpdateIndependantSpeedErrorValues(0, 0);
            asservSpeedDisplay.UpdateIndependantSpeedCorrectionValues(0, 0, 0, 0, 0, 0);
            asservSpeedDisplay.UpdateIndependantSpeedCorrectionGains(0, 0, 0, 0, 0, 0);
            
            asservSpeedDisplay.UpdateIndependantSpeedCorrectionLimits(0, 0, 0, 0, 0, 0);
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            //robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);

            for (int i = 0; i < e.Data.Length; i++)
            {
                DecodeMessage(e.Data[i]);
            }
            //serialPort1.WriteLine(robot.receivedText.ToString());
        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            #region Old Code
            //if (buttonEnvoyer.Background == Brushes.RoyalBlue)
            //{
            //    buttonEnvoyer.Background = Brushes.Beige; //Quand on clique ça change la couleur du bouton, pour la 1, si on appui pls fois ça change le premier coup puis pas après
            //}
            //else
            //{
            //    buttonEnvoyer.Background = Brushes.RoyalBlue;
            //}
            #endregion
            SendMessage();
            //textBoxReception.Text += "Reçu: " + textBoxEmission.Text + "\n";
            //textBoxEmission.Text = "" ;
        }

        private void textBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
            }
        }

        void SendMessage()
        {
            //textBoxReception.Text += "Reçu: " + textBoxEmission.Text + "\n";
            serialPort1.Write(textBoxEmission.Text);
            textBoxEmission.Text = "";
        }

        private void buttonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Text = "";
        }

        private void buttonTest_Click(object sender, RoutedEventArgs e)
        {
            #region Old Code
            //for (int i = 0; i < 20; i++) {
            //byteList[i] = (byte)(2 * i);
            //}

            //serialPort1.Write(byteList, 0, byteList.Length);
            #endregion

            byte[] array = Encoding.ASCII.GetBytes("Bonjour");
            UartEncodeAndSendMessage(0x0080, array.Length, array);
            byte[] arrayLED = { 3, 1 };
            UartEncodeAndSendMessage(0x0020, arrayLED.Length, arrayLED);
            byte[] arrayIR = { 10, 21, 14 };
            UartEncodeAndSendMessage(0x0030, arrayIR.Length, arrayIR);
            byte[] arrayVit = { 50, 25 };
            UartEncodeAndSendMessage(0x0040, arrayVit.Length, arrayVit);


        }
        private byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;

            checksum ^= 0xFE;

            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte)(msgFunction);
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
            }

            return checksum;
        }

        private void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] tabTrame = new byte[msgPayloadLength + 6];
            tabTrame[0] = 0xFE;
            tabTrame[1] = (byte)(msgFunction >> 8);
            tabTrame[2] = (byte)(msgFunction);
            tabTrame[3] = (byte)(msgPayloadLength >> 8);
            tabTrame[4] = (byte)(msgPayloadLength);
            int i = 0;
            for (i = 0; i < msgPayloadLength; i++)
            {
                tabTrame[5 + i] = msgPayload[i];
            }
            tabTrame[5 + i] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

            serialPort1.Write(tabTrame, 0, tabTrame.Length);

        }

        public enum StateReception
        {
            Waiting,
            FunctionMSB,
            FunctionLSB,
            PayloadLengthMSB,
            PayloadLengthLSB,
            Payload,
            CheckSum
        }

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = c << 8;
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodedFunction += c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = c << 8;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c;
                    msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    rcvState = StateReception.Payload;
                    break;

                case StateReception.Payload:
                    if(msgDecodedPayloadLength == 0)
                        rcvState = StateReception.CheckSum;
                    if (msgDecodedPayloadLength > 1024)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    else if (msgDecodedPayloadIndex < msgDecodedPayloadLength)
                    {
                        msgDecodedPayload[msgDecodedPayloadIndex] = c;
                        msgDecodedPayloadIndex++;

                        if (msgDecodedPayloadIndex == msgDecodedPayloadLength)
                        {
                            msgDecodedPayloadIndex = 0;
                            rcvState = StateReception.CheckSum;
                        }
                    }
                    break;

                case StateReception.CheckSum:
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    byte receivedChecksum = c;

                    if (calculatedChecksum == receivedChecksum)
                    {
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                                              

                    }
                    else
                    {
                        ProcessDecodedMessage(0x13, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    rcvState = StateReception.Waiting;
                    break;

                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        private void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            // Case Error 404
            if (msgFunction == 0x13)
            {
                byte[] array = Encoding.ASCII.GetBytes(" Error 404");
                textBoxReception.Text += msgPayload;
                foreach (byte b in array)
                {
                    textBoxReception.Text += Convert.ToChar(b);
                }
            }
            
            // Case Ox80
            else if (msgFunction == 0x80)
            {
                foreach (byte b in msgPayload)
                {
                    textBoxReception.Text += Convert.ToChar(b);
                }
                byte[] array = Encoding.ASCII.GetBytes(" : Nyquit");
                foreach (byte b in array)
                {
                    textBoxReception.Text += Convert.ToChar(b);
                }
            }

            // Case 0x20
            else if (msgFunction == 0x20)
            {
                byte[] tabLED = new byte[msgPayloadLength];

                for (int i = 0; i < msgPayloadLength; i++)
                {
                    tabLED[i] = msgPayload[i];
                }
                if (tabLED[0] == 1)
                {
                    if (tabLED[1] == 0)
                    {
                        Led1.IsChecked = false;
                    }
                    else if (tabLED[1] == 1)
                    {
                        Led1.IsChecked = true;
                    }
                }
                else if (tabLED[0] == 2)
                {
                    if (tabLED[1] == 0)
                    {
                        Led2.IsChecked = false;
                    }
                    else if (tabLED[1] == 1)
                    {
                        Led2.IsChecked = true;
                    }
                }
                else if (tabLED[0] == 3)
                {
                    if (tabLED[1] == 0)
                    {
                        Led3.IsChecked = false;
                    }
                    else if (tabLED[1] == 1)
                    {
                        Led3.IsChecked = true;
                    }
                }
            }

            // Case 0x30
            else if (msgFunction == 0x30)
            {
                byte[] tabIR = new byte[msgPayloadLength];

                for (int i = 0; i < msgPayloadLength; i++)
                {
                    tabIR[i] = msgPayload[i];
                }

                IR_ExtremeGauche.Content = " IR Extreme Gauche : " + msgPayload[0] + " cm";
                IR_Gauche.Content = " IR Gauche : " + msgPayload[1] + " cm";
                IR_Centre.Content = " IR Centre : " + msgPayload[2] + " cm";
                IR_Droit.Content = " IR Droite : " + msgPayload[3] + " cm";
                IR_ExtremeDroite.Content = " IR Extreme Droite : " + msgPayload[4] + " cm";


            }

            // Case 0x40
            else if (msgFunction == 0x40)
            {
                byte[] tabVitesse = new byte[msgPayloadLength];

                for (int i = 0; i < msgPayloadLength; i++)
                {
                    tabVitesse[i] = msgPayload[i];
                }
                Vitesse_Gauche.Content = "Vitesse Gauche : " + msgPayload[0] + "%";
                Vitesse_Droit.Content = "Vitesse Droite : " + msgPayload[1] + "%";
            }

            //Case 0x61
            else if (msgFunction == 0x61)
            {
                var tab = msgPayload.Skip(0).Take(4).Reverse().ToArray(); // retournement Big Endian vers Small Endian
                robot.timestamp = BitConverter.ToUInt32(tab, 0);
                robot.positionX0do = BitConverter.ToSingle(msgPayload, 4);
                robot.positionY0do = BitConverter.ToSingle(msgPayload, 8);
                robot.vitesseLineaireFromOdometry = BitConverter.ToSingle(msgPayload, 16);
                robot.vitesseAngulaireFromOdometry = BitConverter.ToSingle(msgPayload, 20);
                robot.vitesseGaucheFromOdometry = BitConverter.ToSingle(msgPayload, 24);
                robot.vitesseDroitFromOdometry = BitConverter.ToSingle(msgPayload, 28);
                //textBoxReception.Text += "Timestamp: " + robot.timestamp.ToString() + "\n";
                textBoxReception.Text = "Bytes recieved : "+ robot.TextToDisplay.Count().ToString();
                textBoxReception.Text += "\nPosition X: " + robot.positionX0do.ToString() + " Postion Y:  " + robot.positionY0do.ToString() + "\n";
                textBoxReception.Text += "vitesse L: " + robot.vitesseLineaireFromOdometry.ToString() + " vitesse A:  " + robot.vitesseAngulaireFromOdometry.ToString() + "\n";
                oscilloPos.AddPointToLine(0, robot.positionX0do, robot.positionY0do);
                oscilloSpeed.AddPointToLine(0, robot.timestamp, robot.vitesseLineaireFromOdometry);

                asservSpeedDisplay.UpdatePolarOdometrySpeed(robot.vitesseLineaireFromOdometry, robot.vitesseAngulaireFromOdometry);
                asservSpeedDisplay.UpdateIndependantOdometrySpeed(robot.vitesseGaucheFromOdometry, robot.vitesseDroitFromOdometry);
            }

            //Case 0x70
            else if (msgFunction == 0x70)
            {
                robot.Kp = BitConverter.ToSingle(msgPayload, 0);
                robot.Ki = BitConverter.ToSingle(msgPayload, 4);
                robot.Kd = BitConverter.ToSingle(msgPayload, 8);
                robot.limP = BitConverter.ToSingle(msgPayload, 12);
                robot.limI = BitConverter.ToSingle(msgPayload, 16);
                robot.limD = BitConverter.ToSingle(msgPayload, 20);

                asservSpeedDisplay.UpdatePolarSpeedCorrectionGains(robot.Kp, robot.KpT, robot.Ki, robot.KiT, robot.Kd, robot.KdT);
                asservSpeedDisplay.UpdatePolarSpeedCorrectionLimits(robot.limP, robot.limPT, robot.limI, robot.limIT, robot.limD, robot.limDT);

            }

            //Case 0x71
            else if (msgFunction == 0x71)
            {
                robot.KpT = BitConverter.ToSingle(msgPayload, 0);
                robot.KiT = BitConverter.ToSingle(msgPayload, 4);
                robot.KdT = BitConverter.ToSingle(msgPayload, 8);
                robot.limPT = BitConverter.ToSingle(msgPayload, 12);
                robot.limIT = BitConverter.ToSingle(msgPayload, 16);
                robot.limDT = BitConverter.ToSingle(msgPayload, 20);

                asservSpeedDisplay.UpdatePolarSpeedCorrectionGains(robot.Kp, robot.KpT, robot.Ki, robot.KiT, robot.Kd, robot.KdT);
                asservSpeedDisplay.UpdatePolarSpeedCorrectionLimits(robot.limP, robot.limPT, robot.limI, robot.limIT, robot.limD, robot.limDT);

            }

          
            //Case 0x72
            else if (msgFunction == 0x72)
            {
                robot.corrXP = BitConverter.ToSingle(msgPayload, 0);
                robot.corrXI = BitConverter.ToSingle(msgPayload, 4);
                robot.corrXD = BitConverter.ToSingle(msgPayload, 8);
                robot.corrTP = BitConverter.ToSingle(msgPayload, 12);
                robot.corrTI = BitConverter.ToSingle(msgPayload, 16);
                robot.corrTD = BitConverter.ToSingle(msgPayload, 20);
                robot.erreurX = BitConverter.ToSingle(msgPayload, 24);
                robot.erreurT = BitConverter.ToSingle(msgPayload, 28);
                asservSpeedDisplay.UpdatePolarSpeedCorrectionValues(robot.corrXP, robot.corrTP, robot.corrXI, robot.corrTI, robot.corrXD, robot.corrTD);
                asservSpeedDisplay.UpdatePolarSpeedErrorValues(robot.erreurX, robot.erreurT);
            }

            //Case 0x90
            else if (msgFunction == 0x90)
            {
                robot.ConsigneLineaire = BitConverter.ToSingle(msgPayload, 0);
                robot.ConsigneAngulaire = BitConverter.ToSingle(msgPayload, 4);
                asservSpeedDisplay.UpdatePolarSpeedConsigneValues(robot.ConsigneLineaire, robot.ConsigneAngulaire);
            }
            //case 0x91
            else if (msgFunction == 0x91)
            {
                robot.CommandeLineaire = BitConverter.ToSingle(msgPayload, 0);
                robot.CommandeAngulaire = BitConverter.ToSingle(msgPayload, 4);
                asservSpeedDisplay.UpdatePolarSpeedCommandValues(robot.CommandeLineaire, robot.CommandeAngulaire);
            }

            if (msgFunction != 0x13)
            {
                    for (int i = 0; i < msgPayloadLength; i++)
                    {
                        robot.TextToDisplay.Enqueue(Convert.ToString(msgPayload[i]));
                    }
            }

        }

        private void Led1_Checked(object sender, RoutedEventArgs e)
        {
            byte[] arrayLED = { 1, 1 };
            UartEncodeAndSendMessage(0x0020, arrayLED.Length, arrayLED);
        }

        private void Led1_Unchecked(object sender, RoutedEventArgs e)
        {
            byte[] arrayLED = { 1, 0 };
            UartEncodeAndSendMessage(0x0020, arrayLED.Length, arrayLED);
        }

        private void Led2_Checked(object sender, RoutedEventArgs e)
        {
            byte[] arrayLED = { 2, 1 };
            UartEncodeAndSendMessage(0x0020, arrayLED.Length, arrayLED);
        }

        private void Led2_Unchecked(object sender, RoutedEventArgs e)
        {
            byte[] arrayLED = { 2, 0 };
            UartEncodeAndSendMessage(0x0020, arrayLED.Length, arrayLED);
        }

        private void Led3_Checked(object sender, RoutedEventArgs e)
        {
            byte[] arrayLED = { 3, 1 };
            UartEncodeAndSendMessage(0x0020, arrayLED.Length, arrayLED);
        }

        private void Led3_Unchecked(object sender, RoutedEventArgs e)
        {
            byte[] arrayLED = { 3, 0 };
            UartEncodeAndSendMessage(0x0020, arrayLED.Length, arrayLED);
        }

        private void buttonPID_Click(object sender, RoutedEventArgs e)
        {
            float Kp_X = 2.0f;
            float Ki_X = 30.0f;
            float Kd_X = 0.0f;
            float LimP_X = 100.0f;
            float LimI_X = 100.0f;
            float LimD_X = 100.0f;

            float Kp_T = 2.0f;
            float Ki_T =30.0f;
            float Kd_T = 0.0f;
            float LimP_T = 100.0f;
            float LimI_T = 100.0f;
            float LimD_T = 100.0f;

            byte[] Kp_byte_X = BitConverter.GetBytes(Kp_X);
            byte[] Ki_byte_X = BitConverter.GetBytes(Ki_X); 
            byte[] Kd_byte_X = BitConverter.GetBytes(Kd_X);
            byte[] LimP_byte_X = BitConverter.GetBytes(LimP_X);
            byte[] LimI_byte_X = BitConverter.GetBytes(LimI_X);
            byte[] LimD_byte_X = BitConverter.GetBytes(LimD_X);

            byte[] Kp_byte_T = BitConverter.GetBytes(Kp_T);
            byte[] Ki_byte_T = BitConverter.GetBytes(Ki_T);
            byte[] Kd_byte_T = BitConverter.GetBytes(Kd_T);
            byte[] LimP_byte_T = BitConverter.GetBytes(LimP_T);
            byte[] LimI_byte_T = BitConverter.GetBytes(LimI_T);
            byte[] LimD_byte_T = BitConverter.GetBytes(LimD_T);

            byte[] parametrePIDX = new byte[24];

            Kp_byte_X.CopyTo(parametrePIDX, 0);
            Ki_byte_X.CopyTo(parametrePIDX, 4);
            Kd_byte_X.CopyTo(parametrePIDX, 8);
            LimP_byte_X.CopyTo(parametrePIDX, 12);
            LimI_byte_X.CopyTo(parametrePIDX, 16);
            LimD_byte_X.CopyTo(parametrePIDX, 20);

            byte[] parametrePIDT = new byte[24];

            Kp_byte_T.CopyTo(parametrePIDT, 0);
            Ki_byte_T.CopyTo(parametrePIDT, 4);
            Kd_byte_T.CopyTo(parametrePIDT, 8);
            LimP_byte_T.CopyTo(parametrePIDT, 12);
            LimI_byte_T.CopyTo(parametrePIDT, 16);
            LimD_byte_T.CopyTo(parametrePIDT, 20);

            UartEncodeAndSendMessage(0x0070, parametrePIDX.Length,parametrePIDX);
            UartEncodeAndSendMessage(0x0071, parametrePIDT.Length, parametrePIDT);

        }
        byte mode = 0;
        private void buttonMode_Click(object sender, RoutedEventArgs e)
        {

            if (buttonMode.Background == Brushes.Orange)
            {
                buttonMode.Background = Brushes.LightSkyBlue;
                buttonMode.Content = "Mode : Auto";
                mode = 0;
            }
            else
            {
                buttonMode.Background = Brushes.Orange;
                buttonMode.Content = "Mode : Asser";
                mode = 1;

                //float Consigne_X = 1.0f;
                //float Consigne_Theta = 0.0f;

                //byte[] Consigne_X_byte = BitConverter.GetBytes(Consigne_X);
                //byte[] Consigne_Theta_byte = BitConverter.GetBytes(Consigne_Theta);

                //byte[] parametreConsigne = new byte[8];

                //Consigne_X_byte.CopyTo(parametreConsigne, 0);
                //Consigne_Theta_byte.CopyTo(parametreConsigne, 4);

                //UartEncodeAndSendMessage(0x0090, parametreConsigne.Length, parametreConsigne);

            }
            byte[] Mode_byte = {mode};
            UartEncodeAndSendMessage(0x0080, Mode_byte.Length, Mode_byte);
        }

        private void textBoxVitesseLineaire_KeyUp(object sender, KeyEventArgs e)
        {
            float linearSpeedConsigne;
            if (e.Key == Key.Enter)
            {
                if (float.TryParse(textBoxVitesseLineaire.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out linearSpeedConsigne))
                {
                    byte[] ConsigneL_byte = BitConverter.GetBytes(linearSpeedConsigne);
                    byte[] parametreConsigne = new byte[4];
                    ConsigneL_byte.CopyTo(parametreConsigne, 0);
                    UartEncodeAndSendMessage(0x0090, parametreConsigne.Length, parametreConsigne);
                    textBoxVitesseLineaire.Text = "";

                }
            }
        }

        private void textBoxVitesseAngulaire_KeyUp(object sender, KeyEventArgs e)
        {
            float angularSpeedConsigne;
            if (e.Key == Key.Enter)
            {
                if (float.TryParse(textBoxVitesseAngulaire.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out angularSpeedConsigne))
                {
                    byte[] ConsigneA_byte = BitConverter.GetBytes(angularSpeedConsigne);
                    byte[] parametreConsigne = new byte[4];
                    ConsigneA_byte.CopyTo(parametreConsigne, 0);
                    UartEncodeAndSendMessage(0x0091, parametreConsigne.Length, parametreConsigne);
                    textBoxVitesseAngulaire.Text = "";
                }
            }
        }
    }
}
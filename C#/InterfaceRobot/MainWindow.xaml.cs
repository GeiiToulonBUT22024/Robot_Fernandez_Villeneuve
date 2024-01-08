using System;
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
            serialPort1 = new ReliableSerialPort("COM9", 115200, Parity.None, 8, StopBits.One);
            serialPort1.OnDataReceivedEvent += SerialPort1_DataReceived;
            serialPort1.Open();

            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
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
            while (robot.byteListReceived.Count != 0)
            {
                byte b = robot.byteListReceived.Dequeue();
                //textBoxReception.Text += "0x" + b.ToString("X2") + " ";
                textBoxReception.Text += Convert.ToChar(b);
            }
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            robot.receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);

            for (int i = 0; i < e.Data.Length; i++)
            {
                //robot.CurrentMessageReceived.Enqueue(e.Data[i]);
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
                    if (c ==0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = c<<8;
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

                    if(msgDecodedPayloadLength > 1024)
                    {
                        rcvState = StateReception.Waiting;
                    }
                    else if(msgDecodedPayloadIndex < msgDecodedPayloadLength)
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
                foreach (byte b in array)
                {
                    robot.byteListReceived.Enqueue(b);
                }
            }
          
            // Case Ox80
            if (msgFunction == 0x80) {
                foreach (byte b in msgPayload)
                {
                    robot.byteListReceived.Enqueue(b);
                }
                byte[] array = Encoding.ASCII.GetBytes(" : Nyquit");
                foreach (byte b in array)
                {
                    robot.byteListReceived.Enqueue(b);
                }
            }
        }
    }
}
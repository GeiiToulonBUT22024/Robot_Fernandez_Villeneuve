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
        ReliableSerialPort serialPort1;
        public string receivedText;
        DispatcherTimer timerAffichage;

        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ReliableSerialPort("COM6", 115200, Parity.None, 8, StopBits.One);
            serialPort1.OnDataReceivedEvent += SerialPort1_DataReceived;
            serialPort1.Open();
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            if(receivedText != ""){
                textBoxReception.Text = receivedText;
            }
        }

        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            receivedText += Encoding.UTF8.GetString(e.Data, 0, e.Data.Length);
        }

        private void buttonEnvoyer_Click(object sender, RoutedEventArgs e)
        {

            //if (buttonEnvoyer.Background == Brushes.RoyalBlue)
            //{
            //    buttonEnvoyer.Background = Brushes.Beige; //Quand on clique ça change la couleur du bouton, pour la 1, si on appui pls fois ça change le premier coup puis pas après
            //}
            //else
            //{
            //    buttonEnvoyer.Background = Brushes.RoyalBlue;
            //}

            textBoxReception.Text += "Reçu: " + textBoxEmission.Text + "\n";
            textBoxEmission.Text = "" ;

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
            textBoxReception.Text += "Reçu: " + textBoxEmission.Text + "\n";
            textBoxEmission.Text = "";

            serialPort1.WriteLine(textBoxEmission.Text);
        }
    }
}
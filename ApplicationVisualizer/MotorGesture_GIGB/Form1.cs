using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
namespace MotorGesture_GIGB
{
    public partial class Form1 : Form
    {
        private const int C_OFFSET_TELEMETRIES = 3;
        public Client remoteServer;
        IPHostEntry localIP = Dns.Resolve(Dns.GetHostName());
        //TODO change image
        Bitmap bitmap = new Bitmap(Application.StartupPath + "\\..\\..\\Images\\needle.png");

        delegate void PrintHandler(string strReception);
        public Form1()
        {
            InitializeComponent();

            remoteServer = new Client(new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp));

            remoteServer.ClientConnected += RemoteServer_ClientConnected;
            remoteServer.DataReceived += RemoteServer_DataReceived;
            remoteServer.ClientDisconnected += RemoteServer_ClientDisconnected;
            remoteServer.ConnectionRefused += RemoteServer_ConnectionRefused;
            txtBox_ServerIP.Text = localIP.AddressList[0].ToString();//"192.168.0.6";//localIP.AddressList[0].ToString();

            pictBox_Wheel.Image = RotateImage(bitmap, 0, pictBox_Wheel.Width, pictBox_Wheel.Height); ;
            /*
            int LineCount = 0;
            foreach (string line in richTextBox1.Lines)
            {
                richTextBox1.Select(richTextBox1.GetFirstCharIndexFromLine(LineCount), line.Length);
                richTextBox1.SelectionColor = (LineCount++ % 2 == 0) ? Color.Red : Color.Green;
            }
            */

        }

        private void btn_Connect_Click(object sender, EventArgs e)
        {
            if (remoteServer.ClientSocket.Connected)
            {
                remoteServer.Disconnect();
            }
            else
            {
                remoteServer.ClientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp); ;
                remoteServer.Connect(txtBox_ServerIP.Text,2600);//localIP.AddressList[0].ToString(), 2600);//"192.168.0.6", 2600);
            }
        }

        private void btn_SendAngle_Click(object sender, EventArgs e)
        {
            try
            {
                int angle = -Int32.Parse(txtBox_Angle.Text);
                if(remoteServer.ClientSocket.Connected)
                    remoteServer.Send_int(angle);
                else
                    MessageBox.Show("Please connect to the server", "Server disconnected", MessageBoxButtons.OK, MessageBoxIcon.Hand);

            }
            catch
            {
                MessageBox.Show("Please input an integer", "wrong input", MessageBoxButtons.OK, MessageBoxIcon.Hand);

            }

        }


        private void RemoteServer_ClientConnected(Client client)
        {
            Console.WriteLine("connected");
            btn_Connect.Text = "Disconnect";
            lblStatus.ForeColor = Color.Green;
            lblStatus.Text = "Connection status : Connected";
        }

        private void RemoteServer_ClientDisconnected(Client client, string message)
        {

            //MessageBox.Show("You have been disconnected ! Window will now close.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            btn_Connect.Text = "Connect";
            lblStatus.ForeColor = Color.Red;
            lblStatus.Text = "Connection status : Disconnected";
            //this.Close();
        }
        
        private void RemoteServer_ConnectionRefused(Client client, string message)
        {
            MessageBox.Show(message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //this.Close();
        }

        private void RemoteServer_DataReceived(Client client, object data)
        {
         
            String str_reception = Encoding.UTF8.GetString((byte[])data);
            try
            {
                PrintHandler p = new PrintHandler(printClientInfo);
                this.Invoke(p, str_reception);
            }
            catch { }
            //TODO catch ?
        }


      
        private void printClientInfo(string strReception)
        {
            string[] parsedString = strReception.Split(';');
            System.Console.WriteLine($"{parsedString.Length} words in text:");

            foreach (var word in parsedString)
            {
                System.Console.WriteLine($"<{word}>");
            }
           

            if(parsedString.Length == C_OFFSET_TELEMETRIES+1)
            {
                int angle = -Int32.Parse(parsedString[0]);
                int errorRegister = Int32.Parse(parsedString[1]);
                int setpoint = -Int32.Parse(parsedString[2]);
                int command_received = Int32.Parse(parsedString[3]);
                double realCommand = 0.0;
                if (command_received > 10000)
                    realCommand = 100.0;
                else if (command_received < -10000)
                    realCommand = -100.0;
                else
                    realCommand = ((double)command_received) / 100.0;
                


                changeLine(richTextBox1, 0, (setpoint.ToString() + "°"));
                changeLine(richTextBox1, 1, (angle.ToString() + "°"));
                changeLine(richTextBox1, 2, (realCommand.ToString()+"%"));
                 
                pictBox_Wheel.Image = RotateImage(bitmap, angle, pictBox_Wheel.Width, pictBox_Wheel.Height); ;

                int LineCount = 0;

                for (int i = 0; i < richTextBox1.Lines.Length - C_OFFSET_TELEMETRIES; i++)
                {
                    if ( (errorRegister & (0x01 << i) ) != 0)
                    {
                        changeLine(richTextBox1, i+ C_OFFSET_TELEMETRIES, "ON");
                        richTextBox1.Select(richTextBox1.GetFirstCharIndexFromLine(i + C_OFFSET_TELEMETRIES), richTextBox1.Lines[C_OFFSET_TELEMETRIES + i].Length);
                        richTextBox1.SelectionColor = Color.Green;
                    }
                    else
                    {
                        changeLine(richTextBox1, i+ C_OFFSET_TELEMETRIES, "OFF");
                        richTextBox1.Select(richTextBox1.GetFirstCharIndexFromLine(i + C_OFFSET_TELEMETRIES), richTextBox1.Lines[C_OFFSET_TELEMETRIES + i].Length);
                        richTextBox1.SelectionColor = Color.Red;
                    }
                }


            }
        }
        
        void changeLine(RichTextBox RTB, int line, string text)
        {
            int s1 = RTB.GetFirstCharIndexFromLine(line);
            int s2 = line < RTB.Lines.Count() - 1 ?
                      RTB.GetFirstCharIndexFromLine(line + 1) - 1 :
                      RTB.Text.Length;
            RTB.Select(s1, s2 - s1);
            RTB.SelectedText = text;
        }
        
        private Bitmap RotateImage(Bitmap bmp, float angle,int w,int h)
        {
            Bitmap rotatedImage = new Bitmap(w, h);
            rotatedImage.SetResolution(bmp.HorizontalResolution, bmp.VerticalResolution);

            using (Graphics g = Graphics.FromImage(rotatedImage))
            {
                // Set the rotation point to the center in the matrix
                g.TranslateTransform(bmp.Width / 2, bmp.Height / 2);
                // Rotate
                g.RotateTransform(angle);
                // Restore rotation point in the matrix
                g.TranslateTransform(-bmp.Width / 2, -bmp.Height / 2);
                // Draw the image on the bitmap
                g.DrawImage(bmp, new Point(0, 0));
            }

            return rotatedImage;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}

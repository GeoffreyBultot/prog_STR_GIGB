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
        private const int C_OFFSET_TELEMETRIES = 4;
        private const int C_OFFSET_MOTOR_STATE = C_OFFSET_TELEMETRIES + 4;

        public Client remoteServer;
        public Socket commSocket;
        IPHostEntry localIP = Dns.Resolve(Dns.GetHostName());
        //TODO change image
        Bitmap bitmap = new Bitmap(Application.StartupPath + "\\..\\..\\Images\\needle.png");

        delegate void PrintHandler(string strReception);
        public Form1()
        {
            InitializeComponent();

            //remoteServer = new Client(new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp));
            commSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            remoteServer = new Client();

            remoteServer.ClientConnected += RemoteServer_ClientConnected;
            remoteServer.DataReceived += RemoteServer_DataReceived;
            remoteServer.ClientDisconnected += RemoteServer_ClientDisconnected;
            remoteServer.ConnectionRefused += RemoteServer_ConnectionRefused;
//            txtBox_ServerIP.Text = localIP.AddressList[0].ToString();//"192.168.0.6";//localIP.AddressList[0].ToString();
            txtBox_ServerIP.Text = localIP.AddressList[1].ToString();//"192.168.0.6";//localIP.AddressList[0].ToString();

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
            // TODO
            // qd la RPi se déco, on voit dans la console : "Thread 0xblabla has excited with code 0"
            // mais ClientSocket.Connected n'est pas mis à jour ==> recrée un socket et là ça part en youks
            // ==> d
            if (remoteServer.ClientSocket.Connected)
            {
                remoteServer.Disconnect();
            }
            else
            {
                //string HostName= ;

                //DoGetHostAddresses("bultot-realtime-rpi");

                /*string HostName = Dns.GetHostName().ToString();

                IPAddress[] IpInHostAddress = Dns.GetHostAddresses(HostName);

                string IPV4Address = IpInHostAddress[1].ToString(); //Default IPV4Address. This might be the ip address you need to retrieve

                string IPV6Address = IpInHostAddress[0].ToString(); //Default Link local IPv6 Address

                remoteServer.ClientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp); ;
                */
                remoteServer.ClientSocket = commSocket;
                remoteServer.Connect(txtBox_ServerIP.Text, 2600);//localIP.AddressList[0].ToString(), 2600);//"192.168.0.6", 2600);
            }
        }

        public static void DoGetHostAddresses(string hostname)
        {

            IPAddress[] ips;

            ips = Dns.GetHostAddresses(hostname);

            Console.WriteLine("GetHostAddresses({0}) returns:", hostname);

            foreach (IPAddress ip in ips)
            {
                Console.WriteLine("    {0}", ip);
            }
        }

        private void btn_SendAngle_Click(object sender, EventArgs e)
        {
            try
            {
                int angle = Int32.Parse(txtBox_Angle.Text);
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


            if (parsedString.Length == C_OFFSET_TELEMETRIES)
            {
                int angle = Int32.Parse(parsedString[0]);
                int errorRegister = Int32.Parse(parsedString[1]);
                int setpoint = Int32.Parse(parsedString[2]);
                int command_received = Int32.Parse(parsedString[3]);
                int error = setpoint - angle;
                double realCommand = 0.0;
                if (command_received > 10000)
                    realCommand = 100.0;
                else if (command_received < -10000)
                    realCommand = -100.0;
                else
                    realCommand = ((double)command_received) / 100.0;


                changeLine(richTextBox_Telemetries, 0, (setpoint.ToString() + "°"));
                changeLine(richTextBox_Telemetries, 1, (angle.ToString() + "°"));
                changeLine(richTextBox_Telemetries, 2, (error.ToString() + "°"));
                changeLine(richTextBox_Telemetries, 3, (realCommand.ToString() + "%"));
                //Select command line
                richTextBox_Telemetries.Select(richTextBox_Telemetries.GetFirstCharIndexFromLine(3), richTextBox_Telemetries.Lines[3].Length);
                if ( Math.Abs(realCommand) > 75 ){richTextBox_Telemetries.SelectionColor = Color.Red; }
                else if (Math.Abs(realCommand) > 25 && Math.Abs(realCommand) < 75) { richTextBox_Telemetries.SelectionColor = Color.Orange; }
                else {richTextBox_Telemetries.SelectionColor = Color.Green;}

                //Select angle line
                richTextBox_Telemetries.Select(richTextBox_Telemetries.GetFirstCharIndexFromLine(1), richTextBox_Telemetries.Lines[1].Length);
                float errorPercent = 0;
                if (setpoint == 0)
                    errorPercent = Math.Abs((float)error);
                else
                    errorPercent = Math.Abs((float)error / setpoint);

                Console.WriteLine(errorPercent);
                if (Math.Abs(errorPercent) > 50) { richTextBox_Telemetries.SelectionColor = Color.Red; }
                else if (Math.Abs(errorPercent) > 5 && Math.Abs(errorPercent) < 50) { richTextBox_Telemetries.SelectionColor = Color.Orange; }
                else if (Math.Abs(errorPercent) < 4.99) { richTextBox_Telemetries.SelectionColor = Color.Green; }

                //Select error line
                richTextBox_Telemetries.Select(richTextBox_Telemetries.GetFirstCharIndexFromLine(2), richTextBox_Telemetries.Lines[2].Length);
                if (Math.Abs(errorPercent) > 50) { richTextBox_Telemetries.SelectionColor = Color.Red; }
                else if (Math.Abs(errorPercent) > 5 && Math.Abs(errorPercent) < 50) { richTextBox_Telemetries.SelectionColor = Color.Orange; }
                else if (Math.Abs(errorPercent) < 4.99) { richTextBox_Telemetries.SelectionColor = Color.Green; }

                pictBox_Wheel.Image = RotateImage(bitmap, angle, pictBox_Wheel.Width, pictBox_Wheel.Height); ;

                for (int i = 0; i < (richTextBox_Telemetries.Lines.Length - 1) - C_OFFSET_TELEMETRIES; i++)
                {
                    if ((errorRegister & (0x01 << i)) != 0)
                    {
                        changeLine(richTextBox_Telemetries, i + C_OFFSET_TELEMETRIES, "ON");
                        richTextBox_Telemetries.Select(richTextBox_Telemetries.GetFirstCharIndexFromLine(i + C_OFFSET_TELEMETRIES), richTextBox_Telemetries.Lines[C_OFFSET_TELEMETRIES + i].Length);
                        richTextBox_Telemetries.SelectionColor = Color.Green;
                    }
                    else
                    {
                        changeLine(richTextBox_Telemetries, i + C_OFFSET_TELEMETRIES, "OFF");
                        richTextBox_Telemetries.Select(richTextBox_Telemetries.GetFirstCharIndexFromLine(i + C_OFFSET_TELEMETRIES), richTextBox_Telemetries.Lines[C_OFFSET_TELEMETRIES + i].Length);
                        richTextBox_Telemetries.SelectionColor = Color.Red;
                    }
                }

                if ((errorRegister & (0x01 << C_OFFSET_MOTOR_STATE-C_OFFSET_TELEMETRIES)) == 0)
                {
                    changeLine(richTextBox_Telemetries, C_OFFSET_MOTOR_STATE, "RUNNING");
                    richTextBox_Telemetries.Select(richTextBox_Telemetries.GetFirstCharIndexFromLine(C_OFFSET_MOTOR_STATE), richTextBox_Telemetries.Lines[C_OFFSET_MOTOR_STATE].Length);
                    richTextBox_Telemetries.SelectionColor = Color.Green;
                }
                else
                {
                    changeLine(richTextBox_Telemetries, C_OFFSET_MOTOR_STATE, "BLOCKED");
                    richTextBox_Telemetries.Select(richTextBox_Telemetries.GetFirstCharIndexFromLine(C_OFFSET_MOTOR_STATE), richTextBox_Telemetries.Lines[C_OFFSET_MOTOR_STATE].Length);
                    richTextBox_Telemetries.SelectionColor = Color.Red;
                }
                

                changeLine(richTextBox_Telemetries, 0, (setpoint.ToString() + "°"));
                changeLine(richTextBox_Telemetries, 1, (angle.ToString() + "°"));
                changeLine(richTextBox_Telemetries, 2, (error.ToString() + "°"));
                changeLine(richTextBox_Telemetries, 3, (realCommand.ToString() + "%"));
                Console.WriteLine("lalalala");
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

        private void txtBox_Angle_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyData == Keys.Enter)
            {
                btn_SendAngle_Click(sender, e);
            }
        }

        private void txtBox_ServerIP_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if(e.KeyData == Keys.Enter)
            {
                btn_Connect_Click(sender, e);
            }
        }
    }
}

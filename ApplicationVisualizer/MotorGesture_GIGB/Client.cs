using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;

namespace MotorGesture_GIGB
{
    public class Client
    {
        public delegate void ClientConnectedHandler(Client client);
        public delegate void DataSendHandler(Client client);
        public delegate void DataReceivedHandler(Client client, object data);
        public delegate void ClientDisconnectedHandler(Client client, string message);
        public delegate void ConnectionRefusedHandler(Client client, string message);

        public event DataSendHandler DataSent;
        public event DataReceivedHandler DataReceived;
        public event ClientConnectedHandler ClientConnected;
        public event ClientDisconnectedHandler ClientDisconnected;
        public event ConnectionRefusedHandler ConnectionRefused;

        public Socket ClientSocket { get { return clientSocket; } set { clientSocket = value; } }

        private Socket clientSocket;
        public bool ready = false;

        #region Constructors
        /* La construction d'un AsyncClient revient à instancier un objet Socket sur lequel se fera la connexion et 
         * l'échange de connées. Il faudra attendre l'appel à la méthode Connect avant de commencer l'échange de données.
         * Si la construction se fait à partir d'un soket ou d'un autre AsyncClient, on tente tout de suite la réception 
         * de données, au cas ou la connexion était déjà établie.
         */
        public Client()
        {
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        }
        public Client(Socket clientSocket)
        {
            this.clientSocket = clientSocket;
            receiveData();
        }

        public Client(Client client)
        {
            clientSocket = client.ClientSocket;
            receiveData();
        }
        #endregion

        public void Connect(string address, int port)
        {
            if (!clientSocket.Connected)
            {
                IPEndPoint ep = new IPEndPoint(IPAddress.Parse(address), port);
                clientSocket.BeginConnect(ep, clientConnectedCallback, null);
            }
        }
        public void Disconnect()
        {
            if (clientSocket.Connected)
            {
                clientSocket.Close();
            }
        }

        public void Send(string data)
        {
            if (clientSocket.Connected)
            {
                byte[] dataBuffer = Encoding.ASCII.GetBytes(data);//serialize(data);

                try
                {
                    clientSocket.BeginSend(dataBuffer, 0, dataBuffer.Length, SocketFlags.None, dataSendCallback, null);
                }
                catch (SocketException e)
                {
                    onClientDisconnected(e.Message);
                }
            }
        }
        public void Send_int(int data)
        {
            if (clientSocket.Connected)
            {
                //byte[] dataBuffer = Encoding.ASCII.GetBytes(data);//serialize(data);
                byte[] dataBuffer = BitConverter.GetBytes(data);

                try
                {
                    clientSocket.BeginSend(dataBuffer, 0, dataBuffer.Length, SocketFlags.None, dataSendCallback, null);
                }
                catch (SocketException e)
                {
                    onClientDisconnected(e.Message);
                }
            }
        }
        private void receiveData()
        {
            if (clientSocket.Connected)
            {
                ReceiveBuffer receiveBuffer = new ReceiveBuffer();
                clientSocket.BeginReceive(receiveBuffer.tempBuffer, 0, ReceiveBuffer.BufferSize, SocketFlags.None, receiveCallback, receiveBuffer);
            }
        }

        private byte[] serialize(object data)
        {
            BinaryFormatter bin = new BinaryFormatter();
            MemoryStream mem = new MemoryStream();
            bin.Serialize(mem, data);
            byte[] buffer = mem.GetBuffer();
            mem.Close();
            return buffer;
        }

        #region Callbacks

        private void clientConnectedCallback(IAsyncResult ar)
        {
            try
            {
                clientSocket.EndConnect(ar);
            }
            catch (SocketException e)
            {
                onConnectionRefused(e.Message);
            }

            if (clientSocket.Connected)
            {
                receiveData();
                onClientConnected(this);
            }
        }

        private void dataSendCallback(IAsyncResult ar)
        {
            if (clientSocket.Connected)
            {
                clientSocket.EndSend(ar);
                onDataSent(this);
            }
            else
            {
                onClientDisconnected("unable to send data : client disconnected");
            }
        }

        private void receiveCallback(IAsyncResult ar)
        {
            int dataReceivedSize = 0;
            try
            {
                dataReceivedSize = clientSocket.EndReceive(ar);
            }
            catch (Exception e)
            {
                if (!clientSocket.Connected)
                {
                    onClientDisconnected(e.Message);
                }
            }

            ReceiveBuffer receiveBuffer = (ReceiveBuffer)ar.AsyncState;
            if (dataReceivedSize > 0)
            {
                receiveBuffer.Append(dataReceivedSize);
                if (clientSocket.Available > 0)
                    clientSocket.BeginReceive(receiveBuffer.tempBuffer, 0, ReceiveBuffer.BufferSize, SocketFlags.None, receiveCallback, receiveBuffer);
                else
                {
                    object data = receiveBuffer.memStream.ToArray();
                    onDataReceived(data);
                    receiveData();
                }
            }
        }
        #endregion

        #region Raising event methods

        /* Pour les explications, voir AsyncServer.cs
         */

        private void onClientDisconnected(string message)
        {
            if (ClientDisconnected != null)
            {
                if (ClientDisconnected.Target is System.Windows.Forms.Control)
                {
                    ((System.Windows.Forms.Control)ClientDisconnected.Target).Invoke(ClientDisconnected, this, message);
                }
                else
                {
                    ClientDisconnected(this, message);
                }
            }
        }

        private void onConnectionRefused(string message)
        {
            if (ConnectionRefused.Target is System.Windows.Forms.Control)
            {
                ((System.Windows.Forms.Control)ConnectionRefused.Target).Invoke(ConnectionRefused, this, message);
            }
            else
            {
                ConnectionRefused(this, message);
            }
        }

        private void onDataReceived(object data)
        {
            if (DataReceived != null)
            {
                if (DataReceived.Target is System.Windows.Forms.Control)
                {
                    DataReceived(this, data);
                }
                else
                {
                    DataReceived(this, data);
                }
            }
        }

        private void onClientConnected(Client asyncClient)
        {
            if (ClientConnected != null)
            {
                if (ClientConnected.Target is System.Windows.Forms.Control)
                {
                    ((System.Windows.Forms.Control)ClientConnected.Target).Invoke(ClientConnected, this);
                }
                else
                {
                    ClientConnected(this);
                }
            }
        }

        private void onDataSent(Client asyncClient)
        {
            if (DataSent != null)
            {
                if (DataSent.Target is System.Windows.Forms.Control)
                {
                    ((System.Windows.Forms.Control)DataSent.Target).Invoke(DataSent, this);
                }
                else
                {
                    DataSent(this);
                }
            }
        }
        #endregion


    }
}

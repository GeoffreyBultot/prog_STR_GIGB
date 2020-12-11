using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

namespace MotorGesture_GIGB
{
    class ReceiveBuffer
    {
        public const int BufferSize = 4096;
        public byte[] tempBuffer = new byte[BufferSize];
        public MemoryStream memStream = new MemoryStream();

        public int Length
        {
            get { return memStream.GetBuffer().Length; }
        }

        public void Append(int length)
        {
            memStream.Write(tempBuffer, 0, length);
        }

        public object Deserialize()
        {
            BinaryFormatter formatter = new BinaryFormatter();
            object data;
            memStream.Seek(0, SeekOrigin.Begin);

            data = formatter.Deserialize(memStream);

            memStream.Close();

            return data;
        }
    }
}
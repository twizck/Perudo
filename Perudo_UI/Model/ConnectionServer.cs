using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Perudo_UI.Model
{
    public class ConnectionServer
    {
        private const int Port = 3490;
        private const string IpAdress = "127.0.0.1";
        private TcpClient TcpClient = new TcpClient();
        private static Socket socket;

        /// <summary>
        /// Ouverture de la connection au serv (pas de param car tout en dur)
        /// </summary>
        public async Task OpenConnection()
        {
            try
            {
                IPAddress ip = IPAddress.Parse(IpAdress);
                await TcpClient.ConnectAsync(ip, Port);
                if (TcpClient.Connected)
                {
                    NetworkStream stream = TcpClient.GetStream();
                    socket = TcpClient.Client;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                TcpClient.Close();
            }
        }

        /// <summary>
        /// Pour fermer la co au serv
        /// </summary>
        /// <param name="socket">socket</param>
        /// <returns></returns>
        private static bool CloseConnection(Socket socket)
        {
            if (socket != null && socket.Connected)
            {
                try
                {
                    socket.Shutdown(SocketShutdown.Both);
                    socket.Close();
                }
                catch (SocketException ex)
                {
                    Console.WriteLine(ex.ToString());
                }
            }
            return false;
        }
    }
}

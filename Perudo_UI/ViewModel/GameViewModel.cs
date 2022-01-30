using Perudo_UI.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Perudo_UI.ViewModel
{
    public class GameViewModel
    {
        public ConnectionServer ConnectionServer { get; set; }
        public GameViewModel(ConnectionServer connectionServer)
        {
            ConnectionServer = connectionServer;
        }
    }
}

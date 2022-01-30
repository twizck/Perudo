using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Perudo_UI.Model
{
    public class Game
    {
        public virtual List<Player> Players { get; set; }
        public Game(List<Player> players)
        {
            Players = players;
        }
    }
}

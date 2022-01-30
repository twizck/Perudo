using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Perudo_UI.Model
{
    public class Player
    {
        /// <summary>
        /// Definition of a player
        /// </summary>
        [DataMember(Name="id")]
        public int Id { get; set; }

        [DataMember(Name = "name")]
        public string Name { get; set; }

        [DataMember(Name = "numberOfDice")]
        public int NumberOfDice { get; set; }

        [DataMember(Name = "isReady")]
        public bool IsReady { get; set; }

        [DataMember(Name = "diceList")]
        public List<Dice> Dices { get; set; }

        // ctor init avec juste id le rest null ou false
        public Player(int id)
        {
            Id = id;
            IsReady = false;
            Name = null;
            Dices = new List<Dice>();
            NumberOfDice = 0;
        }

        // ctor avec les vrais autres params
        public Player(string name, List<Dice> dices)
        {
            IsReady = false;
            Name = name;
            Dices = dices;
            NumberOfDice = Dices.Count;
        }
    }
}

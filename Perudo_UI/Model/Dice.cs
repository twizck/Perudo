using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Perudo_UI.Model
{
    public class Dice
    {
        /// <summary>
        /// Definition of a dice
        /// </summary>
        public int DiceNumber { get; set; }
        public int Value { get; set; }

        public Dice(int diceNumber, int value)
        {
            DiceNumber = diceNumber;
            Value = value;
        }
    }
}

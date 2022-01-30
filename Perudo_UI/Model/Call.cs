using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Perudo_UI.Model
{
    public class Call
    {
        // mot clé pour appel API
        public string KeyWord { get; set; }

        public Call(string keyword)
        {
            KeyWord = keyword;
        }
    }
}

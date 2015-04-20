using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace App1.Model
{
    public class Album
    {
        public String Name { get; set; }
        public String Author { get; set; }
        public String Icon { get; set; }

    }

    public class Albums : ObservableCollection<Album>
    {
        public Albums()
        {
            this.Add(new Album { Name = "D.U.C", Author = "Booba", Icon = "Albums/Booba.png" });
            this.Add(new Album { Name =  "European Soul", Author = "Citizens!", Icon = "Albums/EuropeanSoul.png" });
            this.Add(new Album { Name = "Glitterbug", Author = "Wombats", Icon = "Albums/Glitterbug.png" });
            this.Add(new Album { Name = "Homeland", Author = "Hindi Zhara", Icon = "Albums/Homeland.png" });
            this.Add(new Album { Name = "La Vague", Author = "Izia", Icon = "Albums/LaVague.png" });
            this.Add(new Album { Name = "Into The Wild Life", Author = "Halestorm", Icon = "Albums/IntoTheWildLife.png" });
            this.Add(new Album { Name = "Outside", Author = "Calvin Harris", Icon = "Albums/Outside.png" });
        }
    }
}

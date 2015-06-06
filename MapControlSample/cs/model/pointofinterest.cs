using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Geolocation;
using Windows.Foundation;

namespace MapControlSample.Model
{
    public class PointOfInterest
    {
        public PointOfInterest()
        {
            this.MoreInfo = "At a glance info info about this Point of interest";
            this.NormalizedAnchorPoint = new Point(0.5, 1);
        }
        public string DisplayName { get; set; }
        public Geopoint Location { get; set; }
        public Uri ImageSourceUri { get; set; }
        public string MoreInfo { get; set; }
        public Point NormalizedAnchorPoint { get; set; }
    }
}

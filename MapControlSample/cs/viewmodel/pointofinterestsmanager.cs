using MapControlSample.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Geolocation;

namespace MapControlSample.ViewModel
{
    public class PointOfInterestsManager
    {
        public List<PointOfInterest> FetchPOIs(BasicGeoposition center)
        {
            List<PointOfInterest> pois = new List<PointOfInterest>();
            pois.Add(new PointOfInterest()
            {
                DisplayName = "Place One",
                ImageSourceUri = new Uri("ms-appx:///Assets/MapPin.png", UriKind.RelativeOrAbsolute),
                Location = new Geopoint(new BasicGeoposition()
                {
                    Latitude = center.Latitude + 0.001,
                    Longitude = center.Longitude - 0.001
                })
            });
            pois.Add(new PointOfInterest()
            {
                DisplayName = "Place Two",
                ImageSourceUri = new Uri("ms-appx:///Assets/MapPin.png", UriKind.RelativeOrAbsolute),
                Location = new Geopoint(new BasicGeoposition()
                {
                    Latitude = center.Latitude + 0.001,
                    Longitude = center.Longitude + 0.001
                })
            });
            pois.Add(new PointOfInterest()
            {
                DisplayName = "Place Three",
                ImageSourceUri = new Uri("ms-appx:///Assets/MapPin.png", UriKind.RelativeOrAbsolute),
                Location = new Geopoint(new BasicGeoposition()
                {
                    Latitude = center.Latitude - 0.001,
                    Longitude = center.Longitude - 0.001
                })
            });
            pois.Add(new PointOfInterest()
            {
                DisplayName = "Place Four",
                ImageSourceUri = new Uri("ms-appx:///Assets/MapPin.png", UriKind.RelativeOrAbsolute),
                Location = new Geopoint(new BasicGeoposition()
                {
                    Latitude = center.Latitude - 0.001,
                    Longitude = center.Longitude + 0.001
                })
            });
            return pois;
        }
    }
}

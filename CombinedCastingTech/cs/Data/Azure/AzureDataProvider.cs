using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Web.Syndication;
using CombineCastingTech.Data.Common;

namespace CombineCastingTech.Data.Azure
{
    public class AzureDataProvider : ItemsDataProvider
    {
        private static Dictionary<string, VideoMetaData> s_metaDataList = new Dictionary<string, VideoMetaData>();

        public override List<VideoMetaData> GetAll(int max_results)
        {
            if (s_metaDataList != null)
            {
                Dictionary<string, VideoMetaData> l = new Dictionary<string, VideoMetaData>();
                VideoMetaData video = new VideoMetaData();
                video.Id = "AzureMediaServicesPromo.ism";
                video.PubDate = DateTime.Now;
                video.VideoLink = new Uri("http://amssamples.streaming.mediaservices.windows.net/91492735-c523-432b-ba01-faba6c2206a2/AzureMediaServicesPromo.ism/manifest(format=m3u8-aapl)");
                video.Title = "Azure Media Services Overview";
                video.WebPageLink = new Uri("http://amsplayer.azurewebsites.net/amssamples.html");
                video.Thumbnail = new Uri("http://t3.gstatic.com/images?q=tbn:ANd9GcRoTu81vs8GuQtJBGUZ0pLEiNHVVx3dFwHsu7JCkvGop8Z-98EsPg");
                l.Add(video.Id, video);

                VideoMetaData video2 = new VideoMetaData();
                video2.Id = "BigBuckBunnyTrailer.ism";
                video2.PubDate = DateTime.Now;
                video2.VideoLink = new Uri("http://amssamples.streaming.mediaservices.windows.net/9ead18f1-29f8-453c-8721-4becc00ff611/BigBuckBunnyTrailer.ism/manifest(format=mpd-time-csf)");
                video2.Title = "Big Buck Bunny";
                video2.WebPageLink = new Uri("http://amsplayer.azurewebsites.net/amssamples.html");
                video2.Thumbnail = new Uri("http://t0.gstatic.com/images?q=tbn:ANd9GcQT54UKh0zX5OUDNLRyXOOwt3pqj0lpaLCJebucva2LSV49aGjRfg");
                l.Add(video2.Id, video2);

                VideoMetaData video3 = new VideoMetaData();
                video3.Id = "TearsOfSteel.ism";
                video3.PubDate = DateTime.Now;
                video3.VideoLink = new Uri("http://amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TearsOfSteel.ism/manifest(format=mpd-time-csf)");
                video3.Title = "Tears of Steel";
                video3.WebPageLink = new Uri("http://amsplayer.azurewebsites.net/amssamples.html");
                video3.Thumbnail = new Uri("http://t3.gstatic.com/images?q=tbn:ANd9GcQO0MMO_vXf8Q1zjZWLHa1_566Mf_jty6vJKRi9R-C_0W-gXnNOog");
                l.Add(video3.Id, video3);

                VideoMetaData video4 = new VideoMetaData();
                video4.Id = Guid.NewGuid().ToString();
                video4.PubDate = DateTime.Now;
                video4.VideoLink = new Uri("http://amssamples.streaming.mediaservices.windows.net/b6822ec8-5c2b-4ae0-a851-fd46a78294e9/ElephantsDream.ism/manifest(filtername=FirstFilter,format=mpd-time-csf)");
                video4.Title = "Elephants Dream";
                video4.WebPageLink = new Uri("http://amsplayer.azurewebsites.net/amssamples.html");
                video4.Thumbnail = new Uri("https://orange.blender.org/wp-content/uploads/2006/05/edscore_cover_l.jpg");
                l.Add(video4.Id, video4);


                VideoMetaData video5 = new VideoMetaData();
                video5.Id = Guid.NewGuid().ToString();
                video5.PubDate = DateTime.Now;
                video5.VideoLink = new Uri("http://amssamples.streaming.mediaservices.windows.net/49b57c87-f5f3-48b3-ba22-c55cfdffa9cb/Sintel.ism/manifest(format=m3u8-aapl)");
                video5.Title = "Sintel";
                video5.WebPageLink = new Uri("http://amsplayer.azurewebsites.net/amssamples.html");
                video5.Thumbnail = new Uri("https://durian.blender.org/wp-content/uploads/2010/05/sintel_trailer_1080.jpg");
                l.Add(video5.Id, video5);

                s_metaDataList = l;
            }
            return s_metaDataList.Values.ToList<VideoMetaData>();
        }

        public override VideoMetaData GetFromID(string ID)
        {
            VideoMetaData video = null;

            if (s_metaDataList.ContainsKey(ID))
                video = s_metaDataList[ID];

            return video;
        }

    }
}

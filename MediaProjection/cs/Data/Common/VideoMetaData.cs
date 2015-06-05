using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CombineCastingTech.Data.Common
{
    public class VideoMetaData
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public DateTime PubDate { get; set; }
        public Uri WebPageLink { get; set; }
        public Uri VideoLink { get; set; }
        public Uri Thumbnail { get; set; }
    }
}

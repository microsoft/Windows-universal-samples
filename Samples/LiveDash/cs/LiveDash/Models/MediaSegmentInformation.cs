using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LiveDash.Models
{
    class MediaSegmentInformation
    {
        private UInt64 timescale;
        private UInt64 startNumber;
        private UInt64 startTimestamp;
        private UInt64 duration;
        private UInt32 bitrate;
        private string representationID;
        
        private string mediaUrlTemplate;

        #region properties

        internal UInt64 Timescale
        {
            get { return timescale; }

            set { timescale = value; }
        }

        internal UInt64 StartNumber
        {
            get { return startNumber; }

            set { startNumber = value; }
        }

        internal UInt64 StartTimestamp
        {
            get { return startTimestamp; }

            set { startTimestamp = value; }
        }

        internal UInt64 Duration
        {
            get { return duration; }

            set { duration = value; }
        }

        internal UInt32 Bitrate
        {
            get { return bitrate; }

            set { bitrate = value; }
        }

        internal string UrlTemplate
        {
            get { return mediaUrlTemplate; }

            set { mediaUrlTemplate = value; }
        }

        internal string RepresentationID
        {
            get { return representationID; }

            set { representationID = value; }
        }

        #endregion

    }
}

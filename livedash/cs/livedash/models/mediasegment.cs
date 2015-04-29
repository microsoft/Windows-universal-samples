using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LiveDash.Models
{
    /// <summary>
    /// Represents a media segment withing a media stream
    /// </summary>
    internal class MediaSegment
    {
        private UInt64 t;
        private UInt64 d;
        private UInt64 s;
        private string url;

        internal MediaSegment()
        {
        }

        #region properties
        internal UInt64 Timestamp
        {
            get { return t; }

            set { t = value; }
        }
        internal UInt64 Duration
        {
            get { return d; }

            set { d = value; }
        }

        internal UInt64 Number
        {
            get { return s; }

            set { s = value; }
        }

        internal string SegmentUrl
        {
            get { return url; }

            set { url = value; }
        }
        #endregion
    }
}

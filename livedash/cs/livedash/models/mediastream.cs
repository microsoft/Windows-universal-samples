using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage.Streams;

namespace LiveDash.Models
{
    /// <summary>
    /// Represents a stream of data, such as audio or video
    /// </summary>
    public class MediaStream
    {
        private string language;
        private string mimeType;
        private string initSegmentUrl;

        private SegmentInformationType m_segmentInfoType;
        private SegmentTemplateIdentifier m_segmentTemplateIdentifier;

        private List<MediaSegment> m_segments;

        private MediaSegmentInformation m_segmentInformation;
        private bool m_canGenerateSegmentsDynamically;

        //  Dynamic information for streaming
        private IBuffer m_initSegmentData = null;
        private bool m_isInitSegmentDownloading = false;

        internal enum SegmentInformationType
        {
            Base,
            Template,
            List
        };

        internal enum SegmentTemplateIdentifier
        {
            Number,
            Time,
            Other
        };

        internal MediaStream()
        {
            m_segments = new List<MediaSegment>();
        }

        #region properties
        internal string Language
        {
            get { return language; }

            set { language = value; }
        }

        internal string MimeType
        {
            get { return mimeType; }

            set { mimeType = value.Replace(" ", string.Empty); }
        }

        internal string InitSegmentUrl
        {
            get{ return initSegmentUrl; }

            set { initSegmentUrl = value; }
        }

        internal List<MediaSegment> Segments
        {
            get { return m_segments; }
        }

        internal IBuffer InitSegmentData
        {
            get { return m_initSegmentData; }
            set { m_initSegmentData = value; }
        }

        internal bool isInitSegmentDownloading
        {
            get { return m_isInitSegmentDownloading; }
            set { m_isInitSegmentDownloading = value; }
        }

        internal SegmentInformationType SegmentInfoType
         {
            get { return m_segmentInfoType; }
            set { m_segmentInfoType = value; }
        }

        internal SegmentTemplateIdentifier SegmentTemplateBase
        {
            get { return m_segmentTemplateIdentifier; }
            set { m_segmentTemplateIdentifier = value; }
        }

        internal MediaSegmentInformation SegmentInformation
        {
            get { return m_segmentInformation; }
            set { m_segmentInformation = value; }
        }

        internal bool CanGenerateSegmentsDynamically
        {
            get { return m_canGenerateSegmentsDynamically; }
            set { m_canGenerateSegmentsDynamically = value; }
        }

        #endregion

        internal void PushBackSegment(MediaSegment segment)
        {
            m_segments.Add(segment);
        }

        internal void PushFrontSegment(MediaSegment segment)
        {
            m_segments.Insert(0, segment);
        }

        internal int GetStartingSegmentIndexForLive(ulong hnsDesiredLiveOffset)
        {
            if (!m_canGenerateSegmentsDynamically)
            {
                int segmentCount = m_segments.Count();

                if (segmentCount > 0)
                {
                    ulong hnsLiveOffset = 0;
                    for (int x = segmentCount - 1; x >= 0; x--)
                    {
                        hnsLiveOffset += m_segments.ElementAt(x).Duration * 10000000 / SegmentInformation.Timescale;
                        if (hnsLiveOffset >= hnsDesiredLiveOffset)
                        {
                            return x;
                        }
                    }
                }
                return 0;
            }
            else
            {
                int x = 0;
                ulong hnsLiveOffset = 0;
                while (hnsLiveOffset < hnsDesiredLiveOffset)
                {
                    hnsLiveOffset += m_segmentInformation.Duration * 10000000 / SegmentInformation.Timescale;
                    x--;
                }
                return x;
            }
            
        }
    }
}

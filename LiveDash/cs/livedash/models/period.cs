using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LiveDash.Models
{
    /// <summary>
    /// Represents a period within a media presentation description
    /// </summary>
    public class Period
    {
        private List<MediaStream> m_streams;
        
        private string id;
        private bool isSelected;
        private TimeSpan duration;
        private TimeSpan start;

        internal Period()
        {
            m_streams = new List<MediaStream>();
        }

        #region properties
        internal string Id
        {
            get
            {
                return id;
            }
            set
            {
                id = value;
            }
        }

        internal bool IsSelected
        {
            get
            {
                return isSelected;
            }
            set
            {
                isSelected = value;
            }
        }

        internal TimeSpan Duration
        {
            get
            {
                return duration;
            }

            set
            {
                duration = value;
            }
        }

        internal TimeSpan Start
        {
            get
            {
                return start;
            }

            set
            {
                start = value;
            }
        }
        #endregion

        internal void AddStream(MediaStream stream)
        {
            m_streams.Add(stream);
        }

        /// <summary>
        /// Gets the stream with the video/mp4 mime type and will
        /// return null otherwise
        /// </summary>
        internal MediaStream GetVideoStream()
        {
            foreach (var stream in m_streams)
            {
                if (stream.MimeType.Equals(Manifest.VideoMimeType))
                {
                    return stream;
                }
            }
            return null;
        }

        /// <summary>
        /// Gets the stream with the audio/mp4 mime type and will
        /// return null otherwise
        /// </summary>
        internal MediaStream GetAudioStream()
        {
            foreach (var stream in m_streams)
            {
                if (stream.MimeType.Equals(Manifest.AudioMimeType))
                {
                    return stream;
                }
            }
            return null;
        }
    }
}

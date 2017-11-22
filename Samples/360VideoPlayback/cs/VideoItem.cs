using System;

namespace _360VideoPlayback
{
    public class VideoItem
    {
        public VideoItem(string title, Uri sourceUri)
        {
            Title = title;
            SourceUri = sourceUri;
        }
        public string Title { get; private set; }
        public Uri SourceUri { get; private set; }
    }
}

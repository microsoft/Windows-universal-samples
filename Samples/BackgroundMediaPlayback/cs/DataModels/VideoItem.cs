using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Media;
using Windows.Media.Playback;

namespace BackgroundMediaPlayback.DataModels
{
    class VideoItem : MediaItem
    {
        public Uri VideoPreviewImageUri { get; set; }

        public override Uri PreviewImageUri
        {
            get
            {
                return VideoPreviewImageUri;
            }

            set
            {
                VideoPreviewImageUri = value;
            }
        }

        public VideoItem()
        {
        }

        public VideoItem(JsonObject json) : base(json)
        {
            if (json.Keys.Contains("videoPreviewImageUri"))
                VideoPreviewImageUri = new Uri(json.GetNamedString("videoPreviewImageUri"));
        }

        public override MediaPlaybackItem ToPlaybackItem()
        {
            var playbackItem = base.ToPlaybackItem();

            var displayProperties = playbackItem.GetDisplayProperties();
            displayProperties.Type = MediaPlaybackType.Video;
            displayProperties.VideoProperties.Title = Title;
            playbackItem.ApplyDisplayProperties(displayProperties);

            return playbackItem;
        }
    }
}

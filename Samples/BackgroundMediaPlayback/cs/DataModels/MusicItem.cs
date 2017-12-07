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
    class MusicItem : MediaItem
    {
        public Uri AlbumArtUri { get; set; }

        public override Uri PreviewImageUri
        {
            get
            {
                return AlbumArtUri;
            }

            set
            {
                AlbumArtUri = value;
            }
        }

        public MusicItem() : base()
        {
        }

        public MusicItem(JsonObject json) : base(json)
        {
            if (json.Keys.Contains("albumArtUri"))
                AlbumArtUri = new Uri(json.GetNamedString("albumArtUri"));
        }

        public override MediaPlaybackItem ToPlaybackItem()
        {
            var playbackItem = base.ToPlaybackItem();

            var displayProperties = playbackItem.GetDisplayProperties();
            displayProperties.Type = MediaPlaybackType.Music;
            displayProperties.MusicProperties.Title = Title;
            playbackItem.ApplyDisplayProperties(displayProperties);

            return playbackItem;
        }
    }
}

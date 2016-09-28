//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Media;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Storage.Streams;

namespace BackgroundMediaPlayback.DataModels
{
    public class MediaItem
    {
        public const string MediaItemIdKey = "mediaItemId";

        Uri previewImageUri;

        public string ItemId { get; set; }

        public string Title { get; set; }

        public Uri MediaUri { get; set; }

        public virtual Uri PreviewImageUri
        {
            get
            {
                return previewImageUri;
            }
            set
            {
                previewImageUri = value;
            }
        }

        public MediaItem()
        {
        }

        public MediaItem(JsonObject json) : this()
        {
            ItemId = json.GetNamedString("id", Guid.NewGuid().ToString());
            Title = json.GetNamedString("title", string.Empty);

            if (json.Keys.Contains("mediaUri"))
                MediaUri = new Uri(json.GetNamedString("mediaUri"));
        }

        public virtual MediaPlaybackItem ToPlaybackItem()
        {
            // Create the media source from the Uri
            var source = MediaSource.CreateFromUri(MediaUri);

            // Create a configurable playback item backed by the media source
            var playbackItem = new MediaPlaybackItem(source);

            // Populate display properties for the item that will be used
            // to automatically update SystemMediaTransportControls when
            // the item is playing.
            var displayProperties = playbackItem.GetDisplayProperties();

            // Populate thumbnail
            if (PreviewImageUri != null)
                displayProperties.Thumbnail = RandomAccessStreamReference.CreateFromUri(PreviewImageUri);

            // Apply properties to the playback item
            playbackItem.ApplyDisplayProperties(displayProperties);

            // It's often useful to save a reference or ID to correlate
            // a particular MediaPlaybackItem with the item from the
            // backing data model. CustomProperties stores serializable
            // types, so here we use the media item's URI as the
            // playback item's unique ID. You are also free to use your own
            // external dictionary if you want to reference non-serializable
            // types.
            source.CustomProperties[MediaItem.MediaItemIdKey] = ItemId;

            return playbackItem;
        }
    }
}

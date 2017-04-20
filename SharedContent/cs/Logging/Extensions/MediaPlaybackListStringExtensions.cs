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

using System.Text;
using Windows.Data.Json;
using Windows.Media.Playback;

namespace SDKTemplate.Logging.Extensions
{
    public static class MediaPlaybackListStringExtensions
    {
        /// <summary>
        /// Provides a JSON object for Current Media PlaybackItem Changed Requested Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this CurrentMediaPlaybackItemChangedEventArgs c)
        {
            var builder = new JsonBuilder("CurrentMediaPlaybackItemChangedEventArgs");
            builder.AddJsonValue("NewItem", c.NewItem?.ToJsonObject());
            builder.AddJsonValue("OldItem", c.OldItem?.ToJsonObject());
            builder.AddString("Reason", c.Reason);
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for Media Playback Item Failed Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this MediaPlaybackItemFailedEventArgs f)
        {
            var builder = new JsonBuilder("MediaPlaybackItemFailedEventArgs");
            builder.AddString("Error", f.Error);
            builder.AddJsonValue("Item", f.Item?.ToJsonObject());
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for Media Playback Item Opened Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this MediaPlaybackItemOpenedEventArgs o)
        {
            var builder = new JsonBuilder("MediaPlaybackItemOpenedEventArgs");
            builder.AddJsonValue("Item", o.Item?.ToJsonObject());
            return builder.GetJsonObject();
        }
    }
}

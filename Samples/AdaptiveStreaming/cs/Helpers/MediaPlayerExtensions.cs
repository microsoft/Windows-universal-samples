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

using Windows.Media.Core;
using Windows.Media.Playback;

namespace SDKTemplate.Helpers
{

    public static class MediaPlayerExtensions
    {
        /// <summary>
        /// Allows for disposal of the underlying MediaSources attached to a MediaPlayer, regardless
        /// of if a MediaSource or MediaPlaybackItem was passed to the MediaPlayer.
        /// 
        /// It is left to the app to implement a clean-up of the other possible IMediaPlaybackSource 
        /// type, which is a MediaPlaybackList.
        /// 
        /// </summary>
        public static void DisposeSource(this MediaPlayer mediaPlayer)
        {
            if (mediaPlayer == null)
                return;

            if (mediaPlayer.Source != null)
            {
                var source = mediaPlayer.Source as MediaSource;
                if (source != null)
                    source.Dispose();

                var item = mediaPlayer.Source as MediaPlaybackItem;
                if (item != null && item.Source != null)
                    item.Source.Dispose();
            };
            mediaPlayer.Source = null;
        }

    }
}

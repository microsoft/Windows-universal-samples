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

using SDKTemplate.Logging.Extensions;
using System;
using Windows.Media.Playback;

namespace SDKTemplate.Logging
{
    public class MediaPlaybackListLogger
    {
        #region Common pattern for all loggers

        private LogView logView;
        private MediaPlaybackList mediaPlaybackList;

        public MediaPlaybackListLogger(LogView _logView, MediaPlaybackList _mediaPlaybackList)
        {
            if (_logView == null)
                throw new ArgumentNullException(nameof(_logView));
            this.logView = _logView;

            if (_mediaPlaybackList == null)
                throw new ArgumentNullException(nameof(_mediaPlaybackList));
            this.mediaPlaybackList = _mediaPlaybackList;

            this.RegisterForMediaPlaybackListEvents();
        }

        public void Dispose()
        {
            this.UnregisterForMediaPlaybackListEvents();

            this.logView = null;
            this.mediaPlaybackList = null;
        }

        #endregion

        #region MediaPlaybackList Event Handlers

        private void RegisterForMediaPlaybackListEvents()
        {
            mediaPlaybackList.CurrentItemChanged += CurrentItemChanged;
            mediaPlaybackList.ItemFailed += ItemFailed;
            mediaPlaybackList.ItemOpened += ItemOpened;
        }

       private void UnregisterForMediaPlaybackListEvents()
        {
            if (mediaPlaybackList == null)
            {
                return;
            }
            mediaPlaybackList.CurrentItemChanged -= CurrentItemChanged;
            mediaPlaybackList.ItemFailed -= ItemFailed;
            mediaPlaybackList.ItemOpened -= ItemOpened;
        }

        private void CurrentItemChanged(MediaPlaybackList sender, CurrentMediaPlaybackItemChangedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Information);
        }

        private void ItemFailed(MediaPlaybackList sender, MediaPlaybackItemFailedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Error);
        }

        private void ItemOpened(MediaPlaybackList sender, MediaPlaybackItemOpenedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Information);
        }

        #endregion


    }
}

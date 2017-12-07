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
using Windows.Media.Streaming.Adaptive;

namespace SDKTemplate.Logging
{
    public class AdaptiveMediaSourceLogger : IDisposable
    {
        #region Common pattern for all loggers

        private LogView logView;
        private AdaptiveMediaSource adaptiveMediaSource;

        public AdaptiveMediaSourceLogger(LogView _logView, AdaptiveMediaSource _adaptiveMediaSource)
        {
            if (_logView == null)
                throw new ArgumentNullException(nameof(_logView));
            this.logView = _logView;

            if (_adaptiveMediaSource == null)
                throw new ArgumentNullException(nameof(_adaptiveMediaSource));
            this.adaptiveMediaSource = _adaptiveMediaSource;

            this.RegisterForAdaptiveMediaSourceEvents();
        }

        public void Dispose()
        {
            this.UnregisterForAdaptiveMediaSourceEvents();

            this.logView = null;
            this.adaptiveMediaSource = null;
        }
        
        #endregion


        #region AdaptiveMediaSource Event Handlers

        private void RegisterForAdaptiveMediaSourceEvents()
        {
            adaptiveMediaSource.Diagnostics.DiagnosticAvailable += DiagnosticAvailable;
            adaptiveMediaSource.DownloadBitrateChanged += DownloadBitrateChanged;
            adaptiveMediaSource.DownloadCompleted += DownloadCompleted;
            adaptiveMediaSource.DownloadFailed += DownloadFailed;
            adaptiveMediaSource.DownloadRequested += DownloadRequested;
            adaptiveMediaSource.PlaybackBitrateChanged += PlaybackBitrateChanged;
        }

        private void UnregisterForAdaptiveMediaSourceEvents()
        {
            if (adaptiveMediaSource == null)
            {
                return;
            }
            adaptiveMediaSource.Diagnostics.DiagnosticAvailable -= DiagnosticAvailable;
            adaptiveMediaSource.DownloadBitrateChanged -= DownloadBitrateChanged;
            adaptiveMediaSource.DownloadCompleted -= DownloadCompleted;
            adaptiveMediaSource.DownloadFailed -= DownloadFailed;
            adaptiveMediaSource.DownloadRequested -= DownloadRequested;
            adaptiveMediaSource.PlaybackBitrateChanged -= PlaybackBitrateChanged;
        }

        private void DiagnosticAvailable(AdaptiveMediaSourceDiagnostics sender, AdaptiveMediaSourceDiagnosticAvailableEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Warning);
        }

        private void DownloadBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadBitrateChangedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Information);
        }

        private void DownloadCompleted(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadCompletedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Verbose);
        }

        private void DownloadFailed(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadFailedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Error);
        }

        private void DownloadRequested(AdaptiveMediaSource sender, AdaptiveMediaSourceDownloadRequestedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Verbose);
        }

        private void PlaybackBitrateChanged(AdaptiveMediaSource sender, AdaptiveMediaSourcePlaybackBitrateChangedEventArgs args)
        {
            logView.Log($"{args.ToJsonObject()}", LogViewLoggingLevel.Information);
        }
        #endregion

    }
}

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
using Windows.Media.Streaming.Adaptive;

namespace SDKTemplate.Logging.Extensions
{
    public static class AdaptiveMediaSourceStringExtensions
    {

        /// <summary>
        /// Provides a JSON formatted string for Download Requested Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourceDownloadRequestedEventArgs e)
        {
            var builder = new JsonBuilder("AdaptiveMediaSourceDownloadRequestedEventArgs");
            builder.AddString("ResourceType", e.ResourceType);
            // builder.AddTimeSpan("Position", e.Position);
            // builder.AddNumber("RequestId", e.RequestId);
            builder.AddString("ResourceUri", e.ResourceUri);
            builder.AddNumber("ResourceByteRangeOffset", e.ResourceByteRangeOffset);
            builder.AddNumber("ResourceByteRangeLength", e.ResourceByteRangeLength);
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for Download Failed Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourceDownloadFailedEventArgs e)
        {
            var builder = new JsonBuilder("AdaptiveMediaSourceDownloadFailedEventArgs");
            builder.AddString("ResourceType", e.ResourceType);
            // builder.AddString("ExtendedError.Message", e.ExtendedError.Message);
            // builder.AddString("ExtendedError.HResult", e.ExtendedError.HResult.ToString("X8"));
            // builder.AddTimeSpan("Position", e.Position);
            // builder.AddNumber("RequestId", e.RequestId));
            builder.AddString("ResourceUri", e.ResourceUri);
            builder.AddNumber("ResourceByteRangeOffset", e.ResourceByteRangeOffset);
            builder.AddNumber("ResourceByteRangeLength", e.ResourceByteRangeLength);
            // builder.AddJsonValue("Statistics", e.Statistics.ToJsonObject());
            builder.AddString("HttpResponseMessage.StatusCode", e.HttpResponseMessage?.StatusCode);
            builder.AddNumber("HttpResponseMessage.Headers.Count", e.HttpResponseMessage?.Headers?.Count);
            builder.AddNumber("HttpResponseMessage.Content.Headers.Count", e.HttpResponseMessage?.Content?.Headers?.Count);
            // ...
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for Download Completed Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourceDownloadCompletedEventArgs e)
        {
            StringBuilder sb = new StringBuilder();
            var builder = new JsonBuilder("AdaptiveMediaSourceDownloadCompletedEventArgs");
            builder.AddString("ResourceType", e.ResourceType);
            // builder.AddTimeSpan("Position", e.Position);
            // builder.AddString("RequestId", e.RequestId));
            builder.AddString("ResourceUri", e.ResourceUri);
            builder.AddNumber("ResourceByteRangeOffset", e.ResourceByteRangeOffset);
            builder.AddNumber("ResourceByteRangeLength", e.ResourceByteRangeLength);
            // builder.AddJsonValue("Statistics", e.Statistics.ToJsonObject());
            builder.AddString("HttpResponseMessage.StatusCode", e.HttpResponseMessage?.StatusCode);
            builder.AddNumber("HttpResponseMessage.Headers.Count", e.HttpResponseMessage?.Headers?.Count);
            builder.AddNumber("HttpResponseMessage.Content.Headers.Count", e.HttpResponseMessage?.Content?.Headers?.Count);
            // ...
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for AdaptiveMediaSource Download Statistics
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourceDownloadStatistics stats)
        {
            var builder = new JsonBuilder("AdaptiveMediaSourceDownloadStatistics");
            builder.AddTimeSpan("TimeToHeadersReceived", stats.TimeToHeadersReceived);
            builder.AddTimeSpan("TimeToFirstByteReceived", stats.TimeToFirstByteReceived);
            builder.AddTimeSpan("TimeToLastByteReceived", stats.TimeToLastByteReceived);
            builder.AddNumber("ContentBytesReceivedCount", stats.ContentBytesReceivedCount);
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for Download Bitrate Changed Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourceDownloadBitrateChangedEventArgs e)
        {
            var builder = new JsonBuilder("AdaptiveMediaSourceDownloadBitrateChangedEventArgs");
            builder.AddNumber("OldValue", e.OldValue);
            builder.AddNumber("NewValue", e.NewValue);
            builder.AddString("Reason", e.Reason);
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for Playback Bitrate Changed Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourcePlaybackBitrateChangedEventArgs e)
        {
            var builder = new JsonBuilder("AdaptiveMediaSourcePlaybackBitrateChangedEventArgs");
            builder.AddNumber("OldValue", e.OldValue);
            builder.AddNumber("NewValue", e.NewValue);
            builder.AddBoolean("AudioOnly", e.AudioOnly);
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for Diagnostic Available Event Args
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourceDiagnosticAvailableEventArgs e)
        {
            var builder = new JsonBuilder("AdaptiveMediaSourceDiagnosticAvailableEventArgs");
            builder.AddString("ResourceType", e.ResourceType);
            builder.AddString("DiagnosticType", e.DiagnosticType);
            // builder.AddTimeSpan("Position", e.Position);
            // builder.AddNumber("RequestId", e.RequestId);
            builder.AddString("ResourceUri", e.ResourceUri);
            builder.AddNumber("ResourceByteRangeOffset", e.ResourceByteRangeOffset);
            builder.AddNumber("ResourceByteRangeLength", e.ResourceByteRangeLength);
            builder.AddNumber("Bitrate", e.Bitrate);
            builder.AddNumber("SegmentId", e.SegmentId);
            return builder.GetJsonObject();
        }

        /// <summary>
        /// Provides a JSON object for AdaptiveMediaSource Correlated Times
        /// </summary>
        public static JsonObject ToJsonObject(this AdaptiveMediaSourceCorrelatedTimes times)
        {
            var builder = new JsonBuilder("AdaptiveMediaSourceCorrelatedTimes");
            builder.AddTimeSpan("Position", times.Position);
            builder.AddTimeSpan("PresentationTimeStamp", times.PresentationTimeStamp);
            builder.AddDateTime("ProgramDateTime", times.ProgramDateTime);
            return builder.GetJsonObject();
        }
    }
}

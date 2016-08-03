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
using Windows.Media.MediaProperties;
using Windows.Media.Playback;

namespace SDKTemplate.Helpers
{
    /// <summary>
    /// This class demonstrates the full range of properties available on the MediaPlaybackItem.
    /// 
    /// </summary>
    public static class MediaPlaybackItemStringExtensions
    {

        public static string ToFormattedString(this MediaPlaybackItem item)
        {
            if (item == null)
                return String.Empty;

            StringBuilder sb = new StringBuilder();

            // MediaPlaybackItem.Source.CustomProperties
            var source = item.Source;
            if (source != null && source.CustomProperties.Any())
            {
                sb.AppendLine();
                sb.AppendLine("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
                sb.AppendLine("Source.CustomProperties:");
                foreach (var prop in source.CustomProperties)
                {
                    sb.AppendLine(prop.Key + ":\t" + prop.Value);
                }
            }

            // MediaPlaybackItem.VideoTracks[].*
            foreach (var track in item.VideoTracks)
            {
                // Note that AdaptiveMediaSource only exposes a single Video track.  Details on bitrates are directly on the AdaptiveMediaSource.
                sb.AppendLine();
                sb.AppendLine("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
                sb.AppendLine("TrackKind:\t" + track.TrackKind);
                sb.AppendLine("Id:\t" + track.Id);
                sb.AppendLine("Label:\t" + track.Label);
                sb.AppendLine("Language:\t" + track.Language);
                sb.AppendLine("Name:\t" + track.Name);
                sb.AppendLine("DecoderStatus:\t" + track.SupportInfo.DecoderStatus);
                sb.AppendLine("MediaSourceStatus:\t" + track.SupportInfo.MediaSourceStatus);
                var encodingProps = track.GetEncodingProperties();
                if (encodingProps != null)
                {
                    sb.AppendLine("Encoding Properties:");
                    sb.AppendLine("Type:\t" + encodingProps.Type);
                    sb.AppendLine("Subtype:\t" + MediaFoundationGuidStringExtensions.ToMFAttributeName(encodingProps.Subtype));
                    sb.AppendLine("Bitrate:\t" + encodingProps.Bitrate);
                    sb.AppendLine(encodingProps.FrameRate.ToFormattedString("FrameRate"));
                    sb.AppendLine("Height:\t" + encodingProps.Height);
                    sb.AppendLine(encodingProps.PixelAspectRatio.ToFormattedString("PixelAspectRatio"));
                    sb.AppendLine("ProfileId:\t" + encodingProps.ProfileId);
                    sb.AppendLine("StereoscopicVideoPackingMode:\t" + encodingProps.StereoscopicVideoPackingMode);
                    sb.AppendLine("Width:\t" + encodingProps.Width);
                    var additionalProperties = encodingProps.Properties;
                    sb.AppendLine(additionalProperties.ToFormattedString());
                }
            }

            // MediaPlaybackItem.AudioTracks[].*
            foreach (var track in item.AudioTracks)
            {
                sb.AppendLine();
                sb.AppendLine("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
                sb.AppendLine("TrackKind:\t" + track.TrackKind);
                sb.AppendLine("Id:\t" + track.Id);
                sb.AppendLine("Label:\t" + track.Label);
                sb.AppendLine("Language:\t" + track.Language);
                sb.AppendLine("Name:\t" + track.Name);
                sb.AppendLine("DecoderStatus:\t" + track.SupportInfo.DecoderStatus);
                sb.AppendLine("MediaSourceStatus:\t" + track.SupportInfo.MediaSourceStatus);
                var encodingProps = track.GetEncodingProperties();
                if (encodingProps != null)
                {
                    sb.AppendLine("Encoding Properties:");
                    sb.AppendLine("Type:\t" + encodingProps.Type);
                    sb.AppendLine("Subtype:\t" + MediaFoundationGuidStringExtensions.ToMFAttributeName(encodingProps.Subtype));
                    sb.AppendLine("Bitrate:\t" + encodingProps.Bitrate);
                    sb.AppendLine("BitsPerSample:\t" + encodingProps.BitsPerSample);
                    sb.AppendLine("ChannelCount:\t" + encodingProps.ChannelCount);
                    sb.AppendLine("SampleRate:\t" + encodingProps.SampleRate);
                    var additionalProperties = encodingProps.Properties;
                    sb.AppendLine(additionalProperties.ToFormattedString());
                }
            }

            // MediaPlaybackItem.TimedMetadataTracks[].*
            foreach (var track in item.TimedMetadataTracks)
            {
                sb.AppendLine();
                sb.AppendLine("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
                sb.AppendLine("TrackKind:\t" + track.TrackKind);
                sb.AppendLine("Id:\t" + track.Id);
                sb.AppendLine("Label:\t" + track.Label);
                sb.AppendLine("Language:\t" + track.Language);
                sb.AppendLine("Name:\t" + track.Name);
                if (track.ActiveCues != null)
                    sb.AppendLine("ActiveCues.Count:\t" + track.ActiveCues.Count);
                if (track.Cues != null)
                    sb.AppendLine("Cues.Count:\t" + track.Cues.Count);
                sb.AppendLine("DispatchType:\t" + track.DispatchType);
                sb.AppendLine("TimedMetadataKind:\t" + track.TimedMetadataKind);
            }
            return sb.ToString();
        }

        static string ToFormattedString(this MediaRatio mediaRatio, string key)
        {
            string retVal = string.Empty;
            if (!String.IsNullOrEmpty(key) && mediaRatio != null)
            {
                retVal = key + ":\t" + mediaRatio.Numerator + "/" + mediaRatio.Denominator;
            }
            return retVal;
        }

        static string ToFormattedString(this MediaPropertySet mediaPropertySet)
        {
            var retVal = new StringBuilder();

            if (mediaPropertySet.Count != 0)
                retVal.AppendLine("Additional Encoding Properties:");
            foreach (var prop in mediaPropertySet)
            {
                // We can also get the various MF_ Guid-based props, some of which are a repeat from above:
                retVal.AppendLine(MediaFoundationGuidStringExtensions.ToMFAttributeName(prop.Key) + ":\t" + MediaFoundationGuidStringExtensions.ToMFAttributeName(prop.Value));

                // Special case for Byte Arrays:
                if (prop.Value.GetType() == typeof(System.Byte[]))
                {
                    var propBytes = prop.Value as System.Byte[];
                    if (propBytes != null && propBytes.Length > 0)
                    {
                        String byteString = "";
                        foreach (var b in propBytes)
                        {
                            byteString += String.Format("{0:X2} ", b);
                        }
                        retVal.AppendLine(MediaFoundationGuidStringExtensions.ToMFAttributeName(prop.Key) + " as Hex:\t" + byteString);
                    }
                }
            }
            return retVal.ToString();
        }
    }


}

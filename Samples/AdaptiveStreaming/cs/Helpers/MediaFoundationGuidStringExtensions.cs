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

namespace SDKTemplate.Helpers
{
    public static class MediaFoundationGuidStringExtensions
    {
        // Translation of selected GUIDs used in MF to their human-readable attribute strings
        // These are in defined the Windows SDK and in various regkeys.
        // Ref: https://msdn.microsoft.com/en-us/library/windows/desktop/dd317906(v=vs.85).aspx

        public static string ToMFAttributeName(Guid guidToFind)
        {
            string retVal = guidToFind.ToString();
            string mfAttribute;
            if (MfGuidDictionary.TryGetValue(guidToFind, out mfAttribute))
            {
                retVal = mfAttribute;
            }
            return retVal;
        }
        public static string ToMFAttributeName(object objectToCheck)
        {
            // Return .ToString as default:
            string retVal = objectToCheck.ToString();

            // Passed object is Guid
            if (objectToCheck is Guid)
            {
                Guid guidToFind = (Guid)objectToCheck;
                retVal = ToMFAttributeName(guidToFind);
            }
            else
            {
                // Passed object is Guid.ToString()
                Guid guidToFind;
                if (Guid.TryParse(retVal, out guidToFind))
                {
                    retVal = ToMFAttributeName(guidToFind);
                }
            }
            return retVal;
        }


        private static Dictionary<Guid, string> MfGuidDictionary = new Dictionary<Guid, string>()
        {
            {new Guid("73646976-0000-0010-8000-00AA00389B71"), "[MEDIATYPE_Video]"},
            {new Guid("73647561-0000-0010-8000-00AA00389B71"), "[MEDIATYPE_Audio]"},
            {new Guid("73747874-0000-0010-8000-00AA00389B71"), "[MEDIATYPE_Text]"},

            {new Guid("3F40F4F0-5622-4FF8-B6D8-A17A584BEE5E"), "[MFVideoFormat_H264_ES]"},

            {new Guid("7632f0e6-9538-4d61-acda-ea29c8c14456"), "[MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION]"},
            {new Guid("bfbabe79-7434-4d1c-94f0-72a3b9e17188"), "[MF_MT_AAC_PAYLOAD_TYPE]"},
            {new Guid("c9173739-5e56-461c-b713-46fb995cb95f"), "[MF_MT_ALL_SAMPLES_INDEPENDENT]"},
            {new Guid("73d1072d-1870-4174-a063-29ff4ff6c11e"), "[MF_MT_AM_FORMAT_TYPE]"},
            {new Guid("1aab75c8-cfef-451c-ab95-ac034b8e1731"), "[MF_MT_AUDIO_AVG_BYTES_PER_SECOND]"},
            {new Guid("f2deb57f-40fa-4764-aa33-ed4f2d1ff669"), "[MF_MT_AUDIO_BITS_PER_SAMPLE]"},
            {new Guid("322de230-9eeb-43bd-ab7a-ff412251541d"), "[MF_MT_AUDIO_BLOCK_ALIGNMENT]"},
            {new Guid("55fb5765-644a-4caf-8479-938983bb1588"), "[MF_MT_AUDIO_CHANNEL_MASK]"},
            {new Guid("37e48bf5-645e-4c5b-89de-ada9e29b696a"), "[MF_MT_AUDIO_NUM_CHANNELS]"},
            {new Guid("a901aaba-e037-458a-bdf6-545be2074042"), "[MF_MT_AUDIO_PREFER_WAVEFORMATEX]"},
            {new Guid("5faeeae7-0290-4c31-9e8a-c534f68d9dba"), "[MF_MT_AUDIO_SAMPLES_PER_SECOND]"},
            {new Guid("799cabd6-3508-4db4-a3c7-569cd533deb1"), "[MF_MT_AVG_BIT_ERROR_RATE]"},
            {new Guid("20332624-fb0d-4d9e-bd0d-cbf6786c102e"), "[MF_MT_AVG_BITRATE]"},
            {new Guid("3afd0cee-18f2-4ba5-a110-8bea502e1f92"), "[MF_MT_COMPRESSED]"},
            {new Guid("b8ebefaf-b718-4e04-b0a9-116775e3321b"), "[MF_MT_FIXED_SIZE_SAMPLES]"},
            {new Guid("c459a2e8-3d2c-4e44-b132-fee5156c7bb0"), "[MF_MT_FRAME_RATE]"},
            {new Guid("1652c33d-d6b2-4012-b834-72030849a37d"), "[MF_MT_FRAME_SIZE]"},
            {new Guid("e2724bb8-e676-4806-b4b2-a8d6efb44ccd"), "[MF_MT_INTERLACE_MODE]"},
            {new Guid("48eba18e-f8c9-4687-bf11-0a74c9f96a8f"), "[MF_MT_MAJOR_TYPE]"},
            {new Guid("3c036de7-3ad0-4c9e-9216-ee6d6ac21cb3"), "[MF_MT_MPEG_SEQUENCE_HEADER]"},
            {new Guid("9aa7e155-b64a-4c1d-a500-455d600b6560"), "[MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY]"},
            {new Guid("261e9d83-9529-4b8f-a111-8b9c950a81a9"), "[MF_MT_MPEG4_SAMPLE_DESCRIPTION]"},
            {new Guid("c6376a1e-8d0a-4027-be45-6d9a0ad39bb6"), "[MF_MT_PIXEL_ASPECT_RATIO]"},
            {new Guid("dad3ab78-1990-408b-bce2-eba673dacc10"), "[MF_MT_SAMPLE_SIZE]"},
            {new Guid("f7e34c9a-42e8-4714-b74b-cb29d72c35e5"), "[MF_MT_SUBTYPE]"},
            {new Guid("b6bc765f-4c3b-40a4-bd51-2535b66fe09d"), "[MF_MT_USER_DATA]"},
            {new Guid("96f66574-11c5-4015-8666-bff516436da7"), "[MF_MT_VIDEO_LEVEL | MF_MT_MPEG2_LEVEL]"},
            {new Guid("ad76a80b-2d5c-4e0b-b375-64e520137036"), "[MF_MT_VIDEO_PROFILE]"},
            {new Guid("c380465d-2271-428c-9b83-ecea3b4a85c1"), "[MF_MT_VIDEO_ROTATION]"},
            {new Guid("a7911d53-12a4-4965-ae70-6eadd6ff0551"), "[MF_NALU_LENGTH_SET]"},
            {new Guid("8f020eea-1508-471f-9da6-507d7cfa40db"), "[MF_PROGRESSIVE_CODING_CONTENT]"},
        };
    }

}

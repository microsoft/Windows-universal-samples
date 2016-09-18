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

namespace SDKTemplate.Models
{
    /// <summary>
    /// A list of manifest URLs, both HLS and DASH; clear, encrypted and PlayReady; VOD and Live.
    /// These are hosted by Azure Media Services, and could expire without notice.
    /// 
    /// It is left to the users of this sample to replace or update this list as needed.
    /// </summary>
    public class AdaptiveContentModel
    {
        public int Id;
        public string DisplayName;
        public Uri ManifestUri;

        public bool Live = false;
        public string Subtitles;  // Out of band subtitles: "Name,LanguageCode,Uri"
        public string Captions;   // Out of band captions: "Name,LanguageCode,Uri"
        public bool Aes = false;
        public string AesToken = "";  // A URL-Encoded Bearer Token
        public bool PlayReady = false;

        private AdaptiveContentModel(int id, string displayName, string manifestUri)
        {
            Id = id;
            DisplayName = displayName;
            ManifestUri = new Uri(manifestUri);
        }

        public override string ToString()
        {
            return $"{Id}: {DisplayName}";
        }

        public static List<AdaptiveContentModel> knownAzureMediaServiceModels = new List<AdaptiveContentModel>
        {
            // Various sample content, most of which is under Creative Commons (CC) license from blender.org
            new AdaptiveContentModel(1, "Azure Media Services Overview (HLS)", "http://amssamples.streaming.mediaservices.windows.net/91492735-c523-432b-ba01-faba6c2206a2/AzureMediaServicesPromo.ism/manifest(format=m3u8-aapl)"),
            new AdaptiveContentModel(101, "Azure Media Services Overview (DASH)", "http://amssamples.streaming.mediaservices.windows.net/91492735-c523-432b-ba01-faba6c2206a2/AzureMediaServicesPromo.ism/manifest(format=mpd-time-csf)"),

            new AdaptiveContentModel(2, "Azure Promo (HLS)", "http://amssamples.streaming.mediaservices.windows.net/69fbaeba-8e92-4740-aedc-ce09ae945073/AzurePromo.ism/manifest(format=m3u8-aapl)"),
            new AdaptiveContentModel(102, "Azure Promo (DASH)", "http://amssamples.streaming.mediaservices.windows.net/69fbaeba-8e92-4740-aedc-ce09ae945073/AzurePromo.ism/manifest(format=mpd-time-csf)"),

            new AdaptiveContentModel(3, "Tears of Steel (HLS) - (CC) mango.blender.org", "http://amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TearsOfSteel.ism/manifest(format=m3u8-aapl)")
            { Subtitles = String.Join(";",
                    "English,en,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-en.vtt",
                    "Spanish,es,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-es.vtt",
                    "French,fr,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-fr.vtt",
                    "Italian,it,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-it.vtt")
            },
            new AdaptiveContentModel(103, "Tears of Steel (DASH) - (CC) mango.blender.org", "http://amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TearsOfSteel.ism/manifest(format=mpd-time-csf)")
            { Subtitles = String.Join(";",
                "English,en,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-en.vtt",
                "Spanish,es,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-es.vtt",
                "French,fr,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-fr.vtt",
                "Italian,it,//amssamples.streaming.mediaservices.windows.net/bc57e088-27ec-44e0-ac20-a85ccbcd50da/TOS-it.vtt")
            },

            new AdaptiveContentModel(4, "Elephant's Dream (HLS) - (CC) orange.blender.org", "http://amssamples.streaming.mediaservices.windows.net/b6822ec8-5c2b-4ae0-a851-fd46a78294e9/ElephantsDream.ism/manifest(format=m3u8-aapl,filtername=FirstFilter)"),
            new AdaptiveContentModel(104, "Elephant's Dream (DASH) - (CC) orange.blender.org", "http://amssamples.streaming.mediaservices.windows.net/b6822ec8-5c2b-4ae0-a851-fd46a78294e9/ElephantsDream.ism/manifest(format=mpd-time-csf,filtername=FirstFilter)"),

            new AdaptiveContentModel(5, "Elephant's Dream (HLS) Multi-Audio - (CC) orange.blender.org", "http://amssamples.streaming.mediaservices.windows.net/55034fb3-11af-43e4-a4de-d1b3ca56c5aa/ElephantsDream_MultiAudio.ism/manifest(format=m3u8-aapl)"),
            new AdaptiveContentModel(105, "Elephant's Dream (DASH) Multi-Audio - (CC) orange.blender.org", "http://amssamples.streaming.mediaservices.windows.net/55034fb3-11af-43e4-a4de-d1b3ca56c5aa/ElephantsDream_MultiAudio.ism/manifest(format=mpd-time-csf)"),

            new AdaptiveContentModel(6, "Sintel (HLS) with voiceless dubbing track - (CC) durian.blender.org", "http://amssamples.streaming.mediaservices.windows.net/f1ee994f-fcb8-455f-a15d-07f6f2081a60/Sintel_MultiAudio.ism/manifest(format=m3u8-aapl)"),
            new AdaptiveContentModel(106, "Sintel (DASH) with voiceless dubbing track - (CC) durian.blender.org", "http://amssamples.streaming.mediaservices.windows.net/f1ee994f-fcb8-455f-a15d-07f6f2081a60/Sintel_MultiAudio.ism/manifest(format=mpd-time-csf)"),

            new AdaptiveContentModel(7, "Silent (HLS) - Copyright 2016, Dolby Laboratories, Inc.", "http://amssamples.streaming.mediaservices.windows.net/d499bfae-117a-40ab-9bd4-036cc575fac3/DolbyDigitalPlus.ism/manifest(format=m3u8-aapl)"),
            new AdaptiveContentModel(107, "Silent (DASH) - Copyright 2016, Dolby Laboratories, Inc.", "http://amssamples.streaming.mediaservices.windows.net/d499bfae-117a-40ab-9bd4-036cc575fac3/DolbyDigitalPlus.ism/manifest(format=mpd-time-csf)"),

            new AdaptiveContentModel(8, "Llama Drama 4K (HLS) - (CC) caminandes.com", "http://amssamples.streaming.mediaservices.windows.net/634cd01c-6822-4630-8444-8dd6279f94c6/CaminandesLlamaDrama4K.ism/manifest(format=m3u8-aapl)"),
            new AdaptiveContentModel(108, "Llama Drama 4K (DASH) - (CC) caminandes.com", "http://amssamples.streaming.mediaservices.windows.net/634cd01c-6822-4630-8444-8dd6279f94c6/CaminandesLlamaDrama4K.ism/manifest(format=mpd-time-csf)"),

            new AdaptiveContentModel(9, "Microsoft HoloLens Demo (HLS)", "http://nimbuspm.origin.mediaservices.windows.net/aed33834-ec2d-4788-88b5-a4505b3d032c/Microsoft's HoloLens Live Demonstration.ism/manifest(format=m3u8-aapl)")
            { Captions = "Indexer,en,http://nimbuspm.origin.mediaservices.windows.net/aed33834-ec2d-4788-88b5-a4505b3d032c/pp4_blog_demo.vtt" },

            new AdaptiveContentModel(109, "Microsoft HoloLens Demo (DASH)", "http://nimbuspm.origin.mediaservices.windows.net/aed33834-ec2d-4788-88b5-a4505b3d032c/Microsoft's HoloLens Live Demonstration.ism/manifest(format=mpd-time-csf)")
            { Captions = "Indexer,en,http://nimbuspm.origin.mediaservices.windows.net/aed33834-ec2d-4788-88b5-a4505b3d032c/pp4_blog_demo.vtt" },
            
            // Live HLS content:
            new AdaptiveContentModel(10, "Azure Media Services 24x7 (HLS, Live) - (CC) blender.org", "http://b028.wpc.azureedge.net/80B028/Samples/a38e6323-95e9-4f1f-9b38-75eba91704e4/5f2ce531-d508-49fb-8152-647eba422aec.ism/manifest(format=m3u8-aapl)")
            { Live = true },

            // Live DASH content (manifest type = dynamic) is not available using the AdaptiveMediaSource: 
            // new AdaptiveContentModel(NA, "Azure Media Services 24x7 (DASH, Live) - (CC) blender.org", "http://b028.wpc.azureedge.net/80B028/Samples/a38e6323-95e9-4f1f-9b38-75eba91704e4/5f2ce531-d508-49fb-8152-647eba422aec.ism/manifest(format=mpd-time-csf)")
            // { Live = true },

            // PlayReady DASH streams need a properly setup MediaProtectionManager:
            new AdaptiveContentModel(110, "Big Buck Bunny (DASH) with DRM (PlayReady/Widevine CENC) - (CC) peach.blender.org", "http://amssamples.streaming.mediaservices.windows.net/622b189f-ec39-43f2-93a2-201ac4e31ce1/BigBuckBunny.ism/manifest(format=mpd-time-csf)")
            { PlayReady = true },
            new AdaptiveContentModel(111, "Tears of Steel Trailer (DASH, PlayReady) - (CC) mango.blender.org", "http://amssamples.streaming.mediaservices.windows.net/de1470b3-7b3c-4902-ab53-d19b37ef3bd7/TearsOfSteelTeaser.ism/manifest(format=mpd-time-csf)")
            { PlayReady = true },
            new AdaptiveContentModel(112, "Sintel Trailer (DASH, PlayReady) - (CC) durian.blender.org", "http://amssamples.streaming.mediaservices.windows.net/bf657309-71d9-4436-b94b-8ac0d2ca222b/SintelTrailer.ism/manifest(format=mpd-time-csf)")
            { PlayReady = true },

            // Live DASH PlayReady content is not available using the AdaptiveMediaSource: 
            // new AdaptiveContentModel(NA, "Azure Media Services 24x7 Live PlayReady/Widevine CENC - (CC) blender.org (DASH)", "http://b028.wpc.azureedge.net/80B028/SampleStream/3f6088ec-9e1f-4db7-b7d9-3ec07183ce7d/e405bd0a-a34f-40e2-b70b-4322c46f5cb7.ism/manifest(format=mpd-time-csf)")
            // { PlayReady = true, Live = true },
            // new AdaptiveContentModel(NA, "Azure Media Services 24x7 Live PlayReady - (CC) blender.org (DASH)", "http://b028.wpc.azureedge.net/80B028/Samples/859b335f-eebc-4ff1-ac03-98e9b614e2c1/0cc164eb-193e-48cb-bc6b-041cc28307e4.ism/manifest(format=mpd-time-csf)")
            // { PlayReady = true, Live = true, },

            // HLS streams with AES-128 CBC encryption:
            new AdaptiveContentModel(11, "Sintel (HLS, AES-128 encryption) - (CC) durian.blender.org", "http://amssamples.streaming.mediaservices.windows.net/49b57c87-f5f3-48b3-ba22-c55cfdffa9cb/Sintel.ism/manifest(format=m3u8-aapl)")
            { Aes = true },
            new AdaptiveContentModel(12, "Azure Media Services 24x7 (HLS, Live, AES-128) - (CC) blender.org", "http://b028.wpc.azureedge.net/80B028/Samples/0e8848ca-1db7-41a3-8867-fe911144c045/d34d8807-5597-47a1-8408-52ec5fc99027.ism/manifest(format=m3u8-aapl)")
            { Aes = true, Live = true },

            // HLS streams with AES-128 CBC encryption which also require token-auth:
            new AdaptiveContentModel(13, "Big Buck Bunny Trailer (HLS, AES-128 with SWT token authorization) - (CC) peach.blender.org", "http://amssamples.streaming.mediaservices.windows.net/9ead18f1-29f8-453c-8721-4becc00ff611/BigBuckBunnyTrailer.ism/manifest(format=m3u8-aapl)")
            {
                Aes = true,
                AesToken = "urn%3amicrosoft%3aazure%3amediaservices%3acontentkeyidentifier=5f5076de-4322-42f7-a533-6265f686d5b9&Audience=urn%3atest&ExpiresOn=4581880130&Issuer=http%3a%2f%2ftestacs.com%2f&HMACSHA256=%2bx77Qo0CeBzP4aCntfe2sVkbKeKguOUNAebHQb53sLc%3d"
            },

            new AdaptiveContentModel(14, "Tears of Steel Trailer (HLS, AES-128 with JWT token authorization) - (CC) mango.blender.org", "http://amssamples.streaming.mediaservices.windows.net/830584f8-f0c8-4e41-968b-6538b9380aa5/TearsOfSteelTeaser.ism/manifest(format=m3u8-aapl)")
            {
                Aes = true,
                AesToken = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJ1cm46bWljcm9zb2Z0OmF6dXJlOm1lZGlhc2VydmljZXM6Y29udGVudGtleWlkZW50aWZpZXIiOiI5ZGRhMGJjYy01NmZiLTQxNDMtOWQzMi0zYWI5Y2M2ZWE4MGIiLCJpc3MiOiJodHRwOi8vdGVzdGFjcy5jb20vIiwiYXVkIjoidXJuOnRlc3QiLCJleHAiOjE3MTA4MDczODl9.lJXm5hmkp5ArRIAHqVJGefW2bcTzd91iZphoKDwa6w8"
            },
        };

        public static IReadOnlyList<AdaptiveContentModel> GetKnownAzureMediaServicesModels()
        {
            return knownAzureMediaServiceModels.AsReadOnly();
        }
    }

}

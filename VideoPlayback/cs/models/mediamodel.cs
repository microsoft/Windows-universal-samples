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
using System.Runtime.Serialization;

namespace VideoPlayback
{
    /// <summary>
    /// Simple representation for songs in a playlist that can be used both for
    /// data model (across processes) and view model (foreground UI)
    /// </summary>
    [DataContract]
    public class MediaModel
    {
        [DataMember]
        public string Title { get; set; }

        [DataMember]
        public Uri MediaUri { get; set; }

        [DataMember]
        public Uri ArtUri { get; set; }
    }
}

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

namespace ScreenCasting.Data.Common
{
    public class VideoMetaData
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public DateTime PubDate { get; set; }        
        public string License { get; set; }
        public Uri WebPageLink { get; set; }
        public Uri VideoLink { get; set; }
        public Uri Thumbnail { get; set; }
    }
}

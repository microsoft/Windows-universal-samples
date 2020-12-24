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

namespace SDKTemplate
{
    /// <summary>
    /// Assorted Media Foundation constants from the Windows SDK C headers
    /// </summary>
    class MediaFoundationConstants
    {
        // https://docs.microsoft.com/en-us/windows/win32/medfound/mf-mt-video-level
        // %ProgramFiles(x86)%\Windows Kits\10\Include\10.0.19041.0\um\mfapi.h
        public static readonly Guid MF_MT_VIDEO_LEVEL = new Guid("{96F66574-11C5-4015-8666-BFF516436DA7}");

        // https://docs.microsoft.com/en-us/windows/win32/api/codecapi/ne-codecapi-eavench264vlevel
        // %ProgramFiles(x86)%\Windows Kits\10\Include\10.0.19041.0\um\codecapi.h
        public enum eAVEncH264VLevel : UInt32
        {
            eAVEncH264VLevel1 = 10,
            eAVEncH264VLevel1_b = 11,
            eAVEncH264VLevel1_1 = 11,
            eAVEncH264VLevel1_2 = 12,
            eAVEncH264VLevel1_3 = 13,
            eAVEncH264VLevel2 = 20,
            eAVEncH264VLevel2_1 = 21,
            eAVEncH264VLevel2_2 = 22,
            eAVEncH264VLevel3 = 30,
            eAVEncH264VLevel3_1 = 31,
            eAVEncH264VLevel3_2 = 32,
            eAVEncH264VLevel4 = 40,
            eAVEncH264VLevel4_1 = 41,
            eAVEncH264VLevel4_2 = 42,
            eAVEncH264VLevel5 = 50,
            eAVEncH264VLevel5_1 = 51,
            eAVEncH264VLevel5_2 = 52,
        };
    }
}

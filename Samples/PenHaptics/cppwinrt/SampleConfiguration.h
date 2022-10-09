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

#pragma once 
#include "pch.h"

namespace winrt::SDKTemplate
{
	std::map<hstring, uint16_t> const& WaveformNamesMap();
	Windows::Devices::Haptics::SimpleHapticsControllerFeedback FindSupportedFeedback(Windows::Devices::Haptics::SimpleHapticsController const& hapticsController, uint16_t waveform);
}

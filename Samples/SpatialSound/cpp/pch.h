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

//
// Header for standard system include files.
//

#pragma once

#include <collection.h>
#include <ppltasks.h>

#include <XAudio2.h>
#include <xapo.h>
#include <hrtfapoapi.h>
#include <DirectXMath.h>
#include <wrl\client.h>
#include <wrl\implements.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <strsafe.h>

using namespace Microsoft::WRL;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;

#include "AudioFileReader.h"
#include "XAudio2Helpers.h"
#include "App.xaml.h"
#include "OmnidirectionalSound.h"
#include "CardioidSound.h"
#include "CustomDecay.h"

#define HRTF_2PI    6.283185307f


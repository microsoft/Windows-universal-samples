//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// Windows Header Files:
#include <windows.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mferror.h>

#include <assert.h>

#include <tchar.h>
#include <Strsafe.h>

#include <windows.foundation.h>
#include <windows.foundation.collections.h>
#include <windows.media.h>
#include <windows.media.capture.h>
#include <windows.media.mediaproperties.h>
#include <windows.networking.sockets.h>

#include <wrl\client.h>
#include <wrl\implements.h>
#include <wrl\ftm.h>
#include <wrl\event.h> 
#include <wrl\wrappers\corewrappers.h>

using namespace Platform;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage::Streams;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Media;
using namespace Windows::Media::Capture;
using namespace Windows::Media::MediaProperties;

#include "Trace.h"
#include <CritSec.h>

#include <collection.h>
#include <ppltasks.h>
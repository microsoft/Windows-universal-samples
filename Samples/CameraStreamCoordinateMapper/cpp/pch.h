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

#define NOMINMAX

#include <wrl.h>
#include <wrl\client.h>
#include <dxgi1_4.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <d2d1effects_2.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <WindowsNumerics.h>
#include <collection.h>
#include <agile.h>
#include <concrt.h>

#include <memory>
#include <shared_mutex>
#include <future>
#include <algorithm>
#include <numeric>
#include <optional>
#include <string>
#include <cmath>

#include <experimental\resumable>
#include <ppltasks.h>
#include <pplawait.h>

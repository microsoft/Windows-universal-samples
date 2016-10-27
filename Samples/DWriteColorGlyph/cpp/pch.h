//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WIN10_RS1

#include <wrl.h>
#include <wrl/client.h>
#include <d3d11_2.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
#include <DirectXMath.h>
#include <memory>
#include <ppltasks.h>
